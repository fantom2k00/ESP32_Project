#include <sys/cdefs.h>
#include "config.h"

#define PUMP_OUT    15
#define GPIO_OUTPUT_PIN_SEL (1ULL<<PUMP_OUT)

#define BT_IN       13
#define GPIO_INPUT_PIN_SEL  (1ULL<<BT_IN)

void gpio_init(void);
void blufi_btn(void);
bool pump_started(unsigned int moisture, unsigned int moist_min, char* day);