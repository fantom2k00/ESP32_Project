#include <sys/cdefs.h>
#include "adc.h"
#include "config.h"
#include "audio.h"
#include "gpio.h"
#include "https.h"
#include "screen.h"

static void display_buddy(void) {
    ssd1306_display_text(0, "       *        ", 16);
    ssd1306_display_text(1, "     *   *      ", 16);
    ssd1306_display_text(2, "   *       *    ", 16);
    ssd1306_display_text(3, " -   Buddy   -  ", 16);
    ssd1306_display_text(4, "   *       *    ", 16);
    ssd1306_display_text(5, "     *   *      ", 16);
    ssd1306_display_text(6, "       *        ", 16);
}

static void display_tutorial(void) {
    ssd1306_display_text(0, " How to Start?  ", 16);
    ssd1306_display_text(1, "                ", 16);
    ssd1306_display_text(2, "Start Bluetooth ", 16);
    ssd1306_display_text(3, "on your Android ", 16);
    ssd1306_display_text(4, "                ", 16);
    ssd1306_display_text(5, "Press Bluetooth ", 16);
    ssd1306_display_text(6, "button on Buddy ", 16);
}

static void display_connecting(void) {
    ssd1306_display_text(0, " Connecting...  ", 16);
    ssd1306_display_text(4, "                ", 16);
    ssd1306_display_text(2, "     _____      ", 16);
    ssd1306_display_text(3, "    /  |  \\    ", 16);
    ssd1306_display_text(4, "   |   |   |    ", 16);
    ssd1306_display_text(5, "   |    \\  |   ", 16);
    ssd1306_display_text(6, "    \\     /    ", 16);
    ssd1306_display_text(7, "     -----      ", 16);
}

_Noreturn void app_main(void)
{
    ESP_LOGI("MAIN", "Initialising 'Buddy'.");
    i2c_master_init(CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

    ESP_LOGI("SCREEN", "Configured!");
    display_buddy();
    audio_init(0);

    ESP_LOGI("BLUFI", "Starting Blufi.");
    display_tutorial();
    blufi_btn();

    ESP_LOGI("WIFI", "Connecting...");
    display_connecting();
    audio_init(1);

    ESP_LOGI("HTTPS", "Starting HTTPS...");
    ssd1306_display_text(0, " Testing HTTPS ", 16);
    https_init();
    audio_init(2);

    ESP_LOGI("TASKS", "Starting ADC and GPIO...");
    adc_init(get_moist_levels());
    gpio_init();

    ssd1306_display_text(0, " Plant's Status ", 16);
    ssd1306_display_text(7, " Plant ID: " CHIP_ID "   ", 16);
    while(1) {
        adc_result();
        insert_sensor_data(get_moisture(), get_light(), get_temperature(), get_distance(), get_status());
        if(pump_started(get_moisture(), get_moist_min(), get_day()))
            insert_water();
    }
}