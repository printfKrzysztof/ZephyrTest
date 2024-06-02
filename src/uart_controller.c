#include <threads_inc.h>
#include <zephyr/kernel.h>

int uart_receive(uint8_t *buffer, size_t length, int timeout)
{
	int received = 0;
	int ret;
	uint8_t c;
	counter_start(counter_dev);
	uint32_t start_time = get_time();
	uint32_t end_time = start_time + counter_us_to_ticks(counter_dev, timeout * 1000);

	while (get_time() < end_time) {
		ret = uart_poll_in(uart2, &c);
		if (ret == 0) {
			buffer[received++] = c;
			if (received == length) {
				counter_stop(counter_dev);
				return 0; // HAL_OK equivalent
			}
		} else if (ret == -1) {
			osDelay(1); // Sleep for a bit to avoid busy-waiting
		}
	}
	counter_stop(counter_dev);
	return -1; // HAL_ERROR equivalent
}

int uart_transmit(const uint8_t *buffer, size_t length, int timeout)
{
	size_t sent = 0;
	uint32_t start_time = get_time();
	uint32_t end_time = start_time + timeout;

	while (sent < length && get_time() < end_time) {
		uart_poll_out(uart2, buffer[sent]);
		sent++;
	}

	return (sent == length) ? 0 : -1; // Return 0 for success (HAL_OK equivalent), -1 for
					  // failure (HAL_ERROR equivalent)
}