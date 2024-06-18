#include <threads_inc.h>
#include <zephyr/kernel.h>

static struct k_msgq uart_msgq;
static uint8_t uart_msgq_buffer[64 * sizeof(uint8_t)];

static void uart_rx_callback(const struct device *uart_dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(uart_dev)) {
		return;
	}

	if (!uart_irq_rx_ready(uart_dev)) {
		return;
	}

	while (uart_fifo_read(uart_dev, &c, 1) == 1) {
		k_msgq_put(&uart_msgq, &c, K_NO_WAIT);
	}
}

int uart_receive(uint8_t *buffer, size_t length, int timeout)
{
	k_msgq_init(&uart_msgq, uart_msgq_buffer, sizeof(uint8_t), 64);

	// Enable UART and set callback
	uart_irq_callback_user_data_set(uart2, uart_rx_callback, NULL);
	uart_irq_rx_enable(uart2);

	for (size_t i = 0; i < length; i++) {
		if (k_msgq_get(&uart_msgq, &buffer[i], K_MSEC(timeout)) != 0) {
			// Disable UART receive
			uart_irq_rx_disable(uart2);
			uart_irq_callback_user_data_set(uart2, NULL, NULL);
			return -1; // Timeout occurred
		}
	}
	// Disable UART receive
	uart_irq_rx_disable(uart2);
	uart_irq_callback_user_data_set(uart2, NULL, NULL);

	return 0; // Reception successful
}

int uart_transmit(const uint8_t *buffer, size_t length, int timeout)
{
	volatile size_t i = 0;
	for (i = 0; i < length; i++) {
		uart_poll_out(uart2, buffer[i]);
	}
	return 0;
}