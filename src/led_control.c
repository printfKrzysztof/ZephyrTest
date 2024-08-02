#include "led_control.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/__assert.h>

#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void led_init(void)
{
	if (!device_is_ready(led.port)) {
		__ASSERT(false, "LED device not ready");
		return;
	}

	int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		__ASSERT(false, "Failed to configure LED pin");
		return;
	}
}

void led_toggle(void)
{
	gpio_pin_toggle_dt(&led);
}