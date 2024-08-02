#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <zephyr/drivers/gpio.h>

void led_init(void);
void led_toggle(void);

#endif // LED_CONTROL_H