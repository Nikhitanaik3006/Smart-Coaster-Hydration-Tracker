#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_timer.h"
#include "nvs_flash.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define DT_PIN GPIO_NUM_1
#define SCK_PIN GPIO_NUM_2

#define OLED_SDA GPIO_NUM_8
#define OLED_SCL GPIO_NUM_9
#define OLED_ADDR 0x3C
#define I2C_PORT I2C_NUM_0

#define EMPTY_BOTTLE_G 12
#define FULL_WATER_G 500
#define SIP_THRESHOLD_G 15
#define BOTTLE_PRESENT_THRESHOLD_G 30

#define BUFFER_SIZE 300
#define GROUP_SIZE 150
#define NOISE_THRESHOLD 2000

static float SCALE_FACTOR = 420.0;

#define SERVICE_UUID 0xFFF0
#define DATA_CHAR_UUID 0xFFF1
#define CMD_CHAR_UUID  0xFFF2

static int32_t OFFSET_RAW = 0;
static int32_t sample_buffer[BUFFER_SIZE];
static int32_t last_good_raw = 0;
static int total_weight_g = 0;
static int water_weight_g = 0;
static int level_percent = 0;
static int sip_count = 0;
static int last_water_weight_g = -1;

static bool connected = false;
static uint16_t conn_handle;
static uint16_t data_chr_handle;
static uint8_t own_addr_type;

static bool time_set = false;
static time_t base_time = 0;
static int64_t base_timer_us = 0;

/* ---------- TIME ---------- */

void get_time_string(char *buf, int len)
{
    if (!time_set) {
        snprintf(buf, len, "00:00 AM");
        return;
    }

    int64_t elapsed_sec = (esp_timer_get_time() - base_timer_us) / 1000000;
    time_t now = base_time + elapsed_sec;

    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    strftime(buf, len, "%I:%M %p", &timeinfo);
}

void set_time_from_string(const char *str)
{
    int hh, mm;
    char ampm[3];

    if (sscanf(str, "%d:%d %2s", &hh, &mm, ampm) != 3) {
        printf("Wrong time format. Use 03:50 AM\n");
        return;
    }

    if (strcmp(ampm, "PM") == 0 && hh != 12) hh += 12;
    if (strcmp(ampm, "AM") == 0 && hh == 12) hh = 0;

    struct tm t = {0};
    t.tm_mday = 1;
    t.tm_mon = 0;
    t.tm_year = 126;
    t.tm_hour = hh;
    t.tm_min = mm;
    t.tm_sec = 0;

    setenv("TZ", "IST-5:30", 1);
    tzset();

    base_time = mktime(&t);
    base_timer_us = esp_timer_get_time();
    time_set = true;

    printf("Time set OK: %s\n", str);
}

/* ---------- OLED ---------- */

void oled_cmd(uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd};
    i2c_master_write_to_device(I2C_PORT, OLED_ADDR, data, 2, pdMS_TO_TICKS(100));
}

void oled_data(uint8_t d)
{
    uint8_t data[2] = {0x40, d};
    i2c_master_write_to_device(I2C_PORT, OLED_ADDR, data, 2, pdMS_TO_TICKS(100));
}

void oled_set_cursor(uint8_t page, uint8_t col)
{
    oled_cmd(0xB0 + page);
    oled_cmd(0x00 + (col & 0x0F));
    oled_cmd(0x10 + ((col >> 4) & 0x0F));
}

void oled_clear(void)
{
    for (int page = 0; page < 8; page++) {
        oled_set_cursor(page, 0);
        for (int col = 0; col < 128; col++) {
            oled_data(0x00);
        }
    }
}

void oled_draw_battery(int percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    int fill = (percent * 90) / 100;

    oled_clear();
    oled_set_cursor(3, 15);

    oled_data(0x7E);

    for (int i = 0; i < 90; i++) {
        oled_data(i < fill ? 0x7E : 0x42);
    }

    oled_data(0x7E);
    oled_data(0x18);
    oled_data(0x18);
}

void oled_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = OLED_SDA,
        .scl_io_num = OLED_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    };

    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);

    vTaskDelay(pdMS_TO_TICKS(100));

    oled_cmd(0xAE);
    oled_cmd(0x20);
    oled_cmd(0x00);
    oled_cmd(0xB0);
    oled_cmd(0xC8);
    oled_cmd(0x00);
    oled_cmd(0x10);
    oled_cmd(0x40);
    oled_cmd(0x81);
    oled_cmd(0xFF);
    oled_cmd(0xA1);
    oled_cmd(0xA6);
    oled_cmd(0xA8);
    oled_cmd(0x3F);
    oled_cmd(0xA4);
    oled_cmd(0xD3);
    oled_cmd(0x00);
    oled_cmd(0xD5);
    oled_cmd(0xF0);
    oled_cmd(0xD9);
    oled_cmd(0x22);
    oled_cmd(0xDA);
    oled_cmd(0x12);
    oled_cmd(0xDB);
    oled_cmd(0x20);
    oled_cmd(0x8D);
    oled_cmd(0x14);
    oled_cmd(0xAF);

    oled_clear();
}

/* ---------- HX711 ---------- */

void wait_us_timer(int us)
{
    int64_t start = esp_timer_get_time();
    while ((esp_timer_get_time() - start) < us) {
        taskYIELD();
    }
}

int32_t read_hx711_raw(void)
{
    uint32_t count = 0;
    int64_t start_time = esp_timer_get_time();

    while (gpio_get_level(DT_PIN) == 1) {
        if ((esp_timer_get_time() - start_time) > 1000000) {
            printf("HX711 not ready\n");
            return 0;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    for (int i = 0; i < 24; i++) {
        gpio_set_level(SCK_PIN, 1);
        wait_us_timer(1);

        count <<= 1;

        gpio_set_level(SCK_PIN, 0);
        wait_us_timer(1);

        if (gpio_get_level(DT_PIN)) count++;
    }

    gpio_set_level(SCK_PIN, 1);
    wait_us_timer(1);
    gpio_set_level(SCK_PIN, 0);
    wait_us_timer(1);

    if (count & 0x800000) count |= 0xFF000000;

    return (int32_t)count;
}

int32_t get_average_raw(int times)
{
    int64_t sum = 0;
    int valid = 0;

    for (int i = 0; i < times; i++) {
        int32_t r = read_hx711_raw();

        if (r != 0) {
            sum += r;
            valid++;
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }

    if (valid == 0) return 0;
    return (int32_t)(sum / valid);
}

int32_t get_stable_raw(void)
{
    int64_t sum = 0;
    int32_t min = 0;
    int32_t max = 0;

    printf("Collecting 300 samples...\n");

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        sample_buffer[i] = read_hx711_raw();

        if(i == 0)
        {
            min = sample_buffer[i];
            max = sample_buffer[i];
        }

        if(sample_buffer[i] < min)
            min = sample_buffer[i];

        if(sample_buffer[i] > max)
            max = sample_buffer[i];

        vTaskDelay(pdMS_TO_TICKS(200));
    }

    printf("Sample collection completed\n");

    printf("Min = %ld\n", min);
    printf("Max = %ld\n", max);
    printf("Fluctuation = %ld\n", (max - min));

    if((max - min) > NOISE_THRESHOLD)
   {
    printf("UNSTABLE DATA - REJECTED\n");

    if(last_good_raw != 0)
        return last_good_raw;

    return get_average_raw(20);
    }

   int32_t reduced[GROUP_SIZE];

     for(int i = 0; i < GROUP_SIZE; i++)
     {
       reduced[i] =
        (sample_buffer[2*i] +
         sample_buffer[2*i + 1]) / 2;
      }

      for(int i = 0; i < GROUP_SIZE; i++)
      {
        sum += reduced[i];
      }

         last_good_raw = (int32_t)(sum / GROUP_SIZE);

         return last_good_raw;
}
void tare_empty_coaster(void)
{
    printf("Keep coaster EMPTY for tare...\n");
    vTaskDelay(pdMS_TO_TICKS(3000));

    OFFSET_RAW = get_average_raw(50);

    total_weight_g = 0;
    water_weight_g = 0;
    level_percent = 0;
    sip_count = 0;
    last_water_weight_g = -1;

    printf("Tare done. OFFSET_RAW = %ld\n", OFFSET_RAW);
}

/* ---------- MEASUREMENT ---------- */

void update_measurements(void)
{
    int32_t stable_raw = get_stable_raw();

    int32_t raw_diff = stable_raw - OFFSET_RAW;

    if (raw_diff < 0) raw_diff = -raw_diff;

    float total_weight = raw_diff / SCALE_FACTOR;

    if (total_weight < 10) total_weight = 0;

    total_weight_g = (int)total_weight;

    if (total_weight_g < BOTTLE_PRESENT_THRESHOLD_G) {
        water_weight_g = 0;
        level_percent = 0;
        return;
    }

    water_weight_g = total_weight_g - EMPTY_BOTTLE_G;

    if (water_weight_g < 0) water_weight_g = 0;
    if (water_weight_g > FULL_WATER_G) water_weight_g = FULL_WATER_G;

    level_percent = (water_weight_g * 100) / FULL_WATER_G;

    if (level_percent > 100) level_percent = 100;
    if (level_percent < 0) level_percent = 0;
}

void update_sip_count(void)
{
    static bool baseline_set = false;
    static int baseline_water_g = 0;

    if (total_weight_g < BOTTLE_PRESENT_THRESHOLD_G)
    {
        return;
    }

    if (!baseline_set)
    {
        baseline_water_g = water_weight_g;
        baseline_set = true;
        printf("Sip baseline set: %dg\n", baseline_water_g);
        return;
    }

    int reduced = baseline_water_g - water_weight_g;

    if (reduced >= SIP_THRESHOLD_G)
    {
        sip_count++;
        printf("SIP DETECTED: %dg\n", reduced);

        baseline_water_g = water_weight_g;
    }

    if (water_weight_g > baseline_water_g + 20)
    {
        baseline_water_g = water_weight_g;
        printf("Refill/baseline updated: %dg\n", baseline_water_g);
    }
}

/* ---------- BLE ---------- */

static int data_access_cb(uint16_t conn_handle,
                          uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt,
                          void *arg)
{
    char time_str[12];
    char msg[64];

    get_time_string(time_str, sizeof(time_str));

    snprintf(msg, sizeof(msg),
             "%s %dg %d%% S:%d",
             time_str,
             total_weight_g,
             level_percent,
             sip_count);

    os_mbuf_append(ctxt->om, msg, strlen(msg));
    return 0;
}

static int cmd_access_cb(uint16_t conn_handle,
                         uint16_t attr_handle,
                         struct ble_gatt_access_ctxt *ctxt,
                         void *arg)
{
    char cmd[32];

    int len = OS_MBUF_PKTLEN(ctxt->om);

    if (len >= sizeof(cmd)) len = sizeof(cmd) - 1;

    os_mbuf_copydata(ctxt->om, 0, len, cmd);
    cmd[len] = '\0';

    printf("Command received: [%s]\n", cmd);

    if (strcmp(cmd, "TARE") == 0) {
        tare_empty_coaster();
    }
  else if (strcmp(cmd, "RESET") == 0) {
    sip_count = 0;
    last_water_weight_g = -1;
    printf("Sip count reset\n");
    }
    else {
        set_time_from_string(cmd);
    }

    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] =
{
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(SERVICE_UUID),

        .characteristics =
        (struct ble_gatt_chr_def[])
        {
            {
                .uuid = BLE_UUID16_DECLARE(DATA_CHAR_UUID),
                .access_cb = data_access_cb,
                .val_handle = &data_chr_handle,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
            },
            {
                .uuid = BLE_UUID16_DECLARE(CMD_CHAR_UUID),
                .access_cb = cmd_access_cb,
                .flags = BLE_GATT_CHR_F_WRITE,
            },
            {0}
        },
    },
    {0}
};

static void advertise(void);

static int gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                connected = true;
                conn_handle = event->connect.conn_handle;
                printf("Mobile connected\n");
            } else {
                advertise();
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            connected = false;
            printf("Mobile disconnected\n");
            advertise();
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
            printf("Notify enabled\n");
            break;

        default:
            break;
    }

    return 0;
}

static void advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;

    memset(&fields, 0, sizeof(fields));

    const char *name = "ESP32_WEIGHT";

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                      &adv_params, gap_event, NULL);
}

static void ble_on_sync(void)
{
    ble_hs_id_infer_auto(0, &own_addr_type);
    ble_svc_gap_device_name_set("ESP32_WEIGHT");
    advertise();
}

static void host_task(void *param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}

/* ---------- MAIN ---------- */

void app_main(void)
{
    printf("\nSMART COASTER STARTING...\n");

    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    gpio_set_direction(DT_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SCK_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(SCK_PIN, 0);

    oled_init();
    oled_draw_battery(0);

    tare_empty_coaster();

    nimble_port_init();

    ble_svc_gap_init();
    ble_svc_gatt_init();

    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);

    ble_hs_cfg.sync_cb = ble_on_sync;

    nimble_port_freertos_init(host_task);

    while (1) {
        update_measurements();
        update_sip_count();

        oled_draw_battery(level_percent);

        char time_str[12];
        char msg[64];

        get_time_string(time_str, sizeof(time_str));

        snprintf(msg, sizeof(msg),
                 "%s %dg %d%% S:%d",
                 time_str,
                 total_weight_g,
                 level_percent,
                 sip_count);

        printf("%s\n", msg);

        if (connected) {
    struct os_mbuf *om =
        ble_hs_mbuf_from_flat(msg,
                              strlen(msg));

    ble_gattc_notify_custom(
        conn_handle,
        data_chr_handle,
        om);
}

printf("Waiting 1 minutes...\n");
fflush(stdout);

vTaskDelay(pdMS_TO_TICKS(1 * 60 * 1000));

printf("10 minutes completed. Taking next reading...\n");
fflush(stdout);

    }
}