/**
 * @file main_thread.c
 * @author Krzysztof Bortnowski (krzysztof.bortnowski@sesto.pl)
 * @brief
 * @version 0.1
 * @date 2024-04-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <stddef.h>
#include <zephyr/toolchain.h>
#include "inttypes.h"
#include "threads_inc.h"
#include "frame_controller.h"
#include "cmsis_os.h"
#include "led_control.h"
// Threads definitions

osThreadId tasks[MAX_THREADS];
#define defalut_stack_size 255

// Those need to be pre-defined to dynamically create threads
static K_THREAD_STACK_ARRAY_DEFINE(stacks_shared, MAX_THREADS, CONFIG_CMSIS_THREAD_MAX_STACK_SIZE);
SYS_BITARRAY_DEFINE_STATIC(bitarray_shared, MAX_THREADS);

osSemaphoreDef(Semaphore);
osMessageQDef(Queue, 100, uint32_t);

void resetValues(uint8_t *buffer_tx, uint8_t *buffer_rx)
{
	for (size_t i = 0; i < MAX_THREADS; i++) {
		for (size_t j = 0; j < MAX_TEST_PER_THREAD; j++) {
			values[i][j] = 0;
			values[i][j] = 0;
		}
	}
	counter_set_top_value(counter_dev, &top_cfg);
	start_flag = 0;

	for (size_t i = 0; i < COMMAND_FRAME_SIZE; i++) {
		buffer_rx[i] = 0;
	}
	for (size_t i = 0; i < SCORE_FRAME_SIZE; i++) {
		buffer_tx[i] = 0;
	}
}

void mainThread(void const *argument)
{

	while (1) {
		led_toggle();
		osDelay(10);
	}
}
