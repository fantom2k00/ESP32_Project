#include "config.h"

#define SREEN_TAG "SCREEN"

#define CONFIG_SDA_GPIO 23
#define CONFIG_SCL_GPIO 22
#define CONFIG_RESET_GPIO 4

#define OLED_CONTROL_BYTE_CMD_STREAM    0x00
#define OLED_CONTROL_BYTE_DATA_STREAM   0x40

#define OLED_CMD_DISPLAY_RAM            0xA4
#define OLED_CMD_DISPLAY_NORMAL         0xA6
#define OLED_CMD_DISPLAY_OFF            0xAE
#define OLED_CMD_DISPLAY_ON             0xAF

#define OLED_CMD_SET_CONTRAST           0x81
#define OLED_CMD_SET_MEMORY_ADDR_MODE   0x20
#define OLED_CMD_SET_PAGE_ADDR_MODE     0x02
#define OLED_CMD_SET_SEGMENT_REMAP_0    0xA0
#define OLED_CMD_SET_SEGMENT_REMAP_1    0xA1
#define OLED_CMD_SET_MUX_RATIO          0xA8
#define OLED_CMD_SET_COM_SCAN_MODE      0xC8
#define OLED_CMD_SET_DISPLAY_OFFSET     0xD3
#define OLED_CMD_SET_COM_PIN_MAP        0xDA

#define OLED_CMD_SET_DISPLAY_CLK_DIV    0xD5
#define OLED_CMD_SET_VCOMH_DESELCT      0xDB
#define OLED_CMD_SET_CHARGE_PUMP        0x8D
#define OLED_CMD_DEACTIVE_SCROLL        0x2E

#define	I2CAddress 0x3C
#define I2C_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000

typedef struct {
	bool _valid;
	int _segLen;
	uint8_t _segs[128];
} PAGE_t;

typedef struct {
	int _address;
	int _width;
	int _height;
	int _pages;
	int _dc;
	bool _scEnable;
	int _scStart;
	int _scEnd;
	int	_scDirection;
	PAGE_t _page[8];
	bool _flip;
} SSD1306_t;

SSD1306_t dev;

void i2c_master_init(int16_t sda, int16_t scl, int16_t reset);
void ssd1306_display_text(int page, const char * text, int text_len);
void ssd1306_clear_screen(void);

