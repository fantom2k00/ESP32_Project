#include "screen.h"
#include "font.h"

static void i2c_init() {
    (&dev)->_width = 128;
    (&dev)->_height = 64;
    (&dev)->_pages = 8;
    if ((&dev)->_height == 32) (&dev)->_pages = 4;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ((&dev)->_address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_OFF, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_MUX_RATIO, true);

    if ((&dev)->_height == 64) i2c_master_write_byte(cmd, 0x3F, true);
    if ((&dev)->_height == 32) i2c_master_write_byte(cmd, 0x1F, true);

    i2c_master_write_byte(cmd, OLED_CMD_SET_DISPLAY_OFFSET, true);
    i2c_master_write_byte(cmd, 0x00, true);
    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);

    if ((&dev)->_flip) {
        i2c_master_write_byte(cmd, OLED_CMD_SET_SEGMENT_REMAP_0, true);
    } else {
        i2c_master_write_byte(cmd, OLED_CMD_SET_SEGMENT_REMAP_1, true);
    }

    i2c_master_write_byte(cmd, OLED_CMD_SET_COM_SCAN_MODE, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_DISPLAY_CLK_DIV, true);
    i2c_master_write_byte(cmd, 0x80, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_COM_PIN_MAP, true);

    if ((&dev)->_height == 64) i2c_master_write_byte(cmd, 0x12, true);
    if ((&dev)->_height == 32) i2c_master_write_byte(cmd, 0x02, true);

    i2c_master_write_byte(cmd, OLED_CMD_SET_CONTRAST, true);
    i2c_master_write_byte(cmd, 0xFF, true);
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_RAM, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_VCOMH_DESELCT, true);
    i2c_master_write_byte(cmd, 0x40, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_MEMORY_ADDR_MODE, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_PAGE_ADDR_MODE, true);
    i2c_master_write_byte(cmd, 0x00, true);

    i2c_master_write_byte(cmd, 0x10, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_CHARGE_PUMP, true);
    i2c_master_write_byte(cmd, 0x14, true);
    i2c_master_write_byte(cmd, OLED_CMD_DEACTIVE_SCROLL, true);
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_NORMAL, true);
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_ON, true);

    i2c_master_stop(cmd);

    esp_err_t espRc = i2c_master_cmd_begin(I2C_NUM, cmd, 10/portTICK_PERIOD_MS);
    if (espRc == ESP_OK) {
        ESP_LOGI(SREEN_TAG, "OLED configured successfully");
    } else {
        ESP_LOGE(SREEN_TAG, "OLED configuration failed. code: 0x%.2X", espRc);
    }
    i2c_cmd_link_delete(cmd);
}

static void i2c_contrast() {
    i2c_cmd_handle_t cmd;
    int _contrast = 255;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ((&dev)->_address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_CONTRAST, true);			// 81
    i2c_master_write_byte(cmd, _contrast, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM, cmd, 10/portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

static void i2c_display_image(int page, int seg, uint8_t *images) {
    i2c_cmd_handle_t cmd;

    if (page >= (&dev)->_pages) return;
    if (seg >= (&dev)->_width) return;

    int _seg = seg;
    uint8_t columLow = _seg & 0x0F;
    uint8_t columHigh = (_seg >> 4) & 0x0F;

    int _page = page;
    if ((&dev)->_flip)
        _page = ((&dev)->_pages - page) - 1;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ((&dev)->_address << 1) | I2C_MASTER_WRITE, true);

    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
    i2c_master_write_byte(cmd, (0x00 + columLow), true);
    i2c_master_write_byte(cmd, (0x10 + columHigh), true);
    i2c_master_write_byte(cmd, 0xB0 | _page, true);

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM, cmd, 10/portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ((&dev)->_address << 1) | I2C_MASTER_WRITE, true);

    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
    i2c_master_write(cmd, images, 8, true);

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM, cmd, 10/portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void i2c_master_init(int16_t sda, int16_t scl, int16_t reset) {
    i2c_config_t i2c_config = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = sda,
            .scl_io_num = scl,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM, &i2c_config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM, I2C_MODE_MASTER, 0, 0, 0));

    if (reset >= 0) {
        gpio_reset_pin(reset);
        gpio_set_direction(reset, GPIO_MODE_OUTPUT);
        gpio_set_level(reset, 0);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_set_level(reset, 1);
    }
    (&dev)->_address = I2CAddress;
    (&dev)->_flip = false;

    i2c_init();
    ssd1306_clear_screen();
    i2c_contrast();
}

void ssd1306_display_text(int page, const char * text, int text_len) {
	if (page >= (&dev)->_pages) return;
	int _text_len = text_len;
	if (_text_len > 16) _text_len = 16;

	uint8_t seg = 0;
	uint8_t image[8];
	for (int i = 0; i < _text_len; i++) {
		memcpy(image, font8x8_basic_tr[(uint8_t)text[i]], 8);
        i2c_display_image(page, seg, image);
		seg = seg + 8;
	}
}

void ssd1306_clear_screen(void)
{
    char space[16];
    memset(space, 0x20, sizeof(space));
    for (int page = 0; page < (&dev)->_pages; page++)
        ssd1306_display_text(page, space, sizeof(space));
}