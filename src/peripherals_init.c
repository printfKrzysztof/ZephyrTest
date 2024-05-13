
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#define TIMER_NODE DT_ALIAS(timer1)
#define UART_NODE  DT_ALIAS(uart2)

static const struct device *timer_dev;
static const struct device *uart_dev;
void timer_init(void)
{
	// timer_dev = device_get_binding(DT_LABEL(TIMER_NODE));
	// if (!timer_dev)
	// {
	//     printk("Error: could not find %s\n", DT_LABEL(TIMER_NODE));
	//     return;
	// }

	// // Timer configuration
	// timer_configured_set(timer_dev, TIMER_CFG_DCS | TIMER_CFG_PCS);
}

void uart_init(void)
{
	// uart_dev = device_get_binding(DT_LABEL(UART_NODE));
	// if (!uart_dev) {
	// 	printk("Error: could not find %s\n", DT_LABEL(UART_NODE));
	// 	return;
	// }

	// UART configuration
	// uart_configure(uart_dev, &uart_cfg);
}