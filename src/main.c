/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>
#include <threads_inc.h>
// #include <zephyr/sys/printk.h>

struct counter_top_cfg top_cfg;
uint32_t values[MAX_THREADS][MAX_TEST_PER_THREAD];
int start_flag;
osSemaphoreId semaphoreHandle;
osMessageQId queueHandle;

const struct device *counter_dev;
struct counter_alarm_cfg alarm_cfg;
const struct device *uart2;

// #define TESTING_BARE_METAL 1

#define TIMER         DT_INST(0, st_stm32_counter)
/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

uint32_t get_time()
{
	uint32_t temp;
	counter_get_value(counter_dev, &temp);
	return temp;
}
int main(void)
{

	counter_dev = DEVICE_DT_GET(TIMER);
	uart2 = DEVICE_DT_GET(DT_NODELABEL(usart2));
	gpio_pin_toggle_dt(&led);

	if (!device_is_ready(uart2)) {

		return 1;
	}

	if (!device_is_ready(led.port)) {

		return 1;
	}

	int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {

		return 1;
	}

	if (!device_is_ready(counter_dev)) {
		return 1;
	}

	top_cfg.ticks = counter_us_to_ticks(counter_dev, 0xFFFFFFFF);
	top_cfg.callback = NULL;
	top_cfg.user_data = NULL;
	top_cfg.flags = 0;

#ifndef TESTING_BARE_METAL
	osThreadDef(mainThread, osPriorityNormal, 1, 1024);
	osThreadId id = osThreadCreate(osThread(mainThread), NULL);
#else
	ret = counter_start(counter_dev);
	if (ret != 0) {
		return 1;
	}
	counter_set_top_value(counter_dev, &top_cfg);

	int i = 0;
	uint32_t time[101];

	// First ten is to check how much does read take

	counter_get_value(counter_dev, &time[i++]);
	counter_get_value(counter_dev, &time[i++]);
	counter_get_value(counter_dev, &time[i++]);
	counter_get_value(counter_dev, &time[i++]);
	counter_get_value(counter_dev, &time[i++]);

	while (1) {
		counter_get_value(counter_dev, &time[i++]);
		if (i == 100) {
			break;
		}
	}

	counter_get_value(counter_dev, &time[i++]);
	counter_stop(counter_dev);

	char text[10];

	for (int j = 0; j < i - 1; j++) {
		snprintf(text, sizeof(text), "%lu", time[j + 1] - time[j]);
		uart_poll_out(uart2, text[0]);
		uart_poll_out(uart2, text[1]);
		uart_poll_out(uart2, ' ');
	}

#endif // !TESTING_BARE_METAL

	return 0;
}
