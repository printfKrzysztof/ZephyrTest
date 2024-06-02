/**
 * @file test2.c
 * @author Krzysztof Bortnowski (krzysztof.bortnowski@sesto.pl)
 * @brief
 * @version 0.1
 * @date 2024-04-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "threads_inc.h"

void semaphoreThread(void const *argument)
{
	const uint8_t data = ((uint8_t *)argument)[0]; // Task number
	const int max = ((uint8_t *)argument)[1];      // Number of measurements per task
	while (!start_flag) {
		osDelay(1); // Forcing task switch so lower priority has a chance to take context
	}

	int i = 0;
	while (1) {
		osSemaphoreWait(semaphoreHandle, osWaitForever);
		counter_get_value(counter_dev, &values[data][i++]);
		osThreadYield(); // Forcing task switch

		osSemaphoreRelease(semaphoreHandle);
		counter_get_value(counter_dev, &values[data][i++]);
		osThreadYield(); // Forcing task switch

		if (i == max) {
			break;
		}
	}

	while (1) {
		osDelay(10); // Forcing delay so that main_thread has a chance to take context
	}
}
