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

	(void)(argument);
	uint8_t buffer_rx[COMMAND_FRAME_SIZE];
	uint8_t buffer_tx[SCORE_FRAME_SIZE];
	while (1) {
		if (uart_receive(buffer_rx, COMMAND_FRAME_SIZE, 1000) == 0) {
			commands_em command;
			uint8_t arg_count, args[4];
			if (DecodeCommandFrame(buffer_rx, &command, &arg_count, args) == 0) {
				switch (command) {
				case CMD_TASK_FORCE_SWITCH:

				{
					resetValues(buffer_tx,
						    buffer_rx); // Reseting all values

					// Argument 1 - Number of threads
					// Argument 2 - Number of measurements per task
					osThreadDef2(forceSwitchThread, osPriorityNormal,
						     MAX_THREADS, defalut_stack_size, stacks_shared,
						     bitarray_shared);
					uint8_t task_args[args[0]][2];
					memset(task_args, 0, sizeof(task_args));

					for (size_t i = 0; i < args[0]; i++) {
						task_args[i][0] = i;
						task_args[i][1] = args[1];

						// Create the task
						tasks[i] = osThreadCreate(
							osThread(forceSwitchThread), task_args[i]);
						if (tasks[i] == NULL) {
							// Handle error: Failed to create
							// task
						}
					}

					counter_start(counter_dev);
					start_flag = 1;
					osDelay(10); // 10 milisecond block for main task

					counter_stop(counter_dev);

					for (size_t i = 0; i < args[0]; i++) // For each task
					{
						osThreadTerminate(tasks[i]);
					}
					// free(cm_thread_forceSwitchThread);
					for (size_t i = 0; i < args[0]; i++) // For each task
					{
						if (CodeScoreFrame(buffer_tx, CMD_TASK_FORCE_SWITCH,
								   (uint16_t)(args[1] * 4),
								   (uint8_t *)(values[i])) == 0) {
							uart_transmit(buffer_tx, SCORE_FRAME_SIZE,
								      1000);
						}
						// osDelay(10);
					}
				} break;

				case CMD_TASK_FORCE_SWITCH_PRIORITY:

				{

					resetValues(buffer_tx,
						    buffer_rx); // Reseting all values

					// Argument 1 - Number of threads Low
					// Argument 2 - Number of threads High
					// Argument 3 - Number of measurements per task

					uint8_t task_count_sum = args[0] + args[1];

					if (task_count_sum >= MAX_THREADS) {
						break;
					}
					uint8_t task_args[MAX_THREADS][2];

					osThreadDef2(forceSwitchPriorityThread,
						     osPriorityBelowNormal, MAX_THREADS,
						     defalut_stack_size, stacks_shared,
						     bitarray_shared);

					for (size_t i = 0; i < task_count_sum; i++) {
						// Create the task
						task_args[i][0] = i;
						task_args[i][1] = args[2];
						if (i < args[0]) {
							os_thread_def_forceSwitchPriorityThread
								.tpriority = osPriorityBelowNormal;

						} else {
							os_thread_def_forceSwitchPriorityThread
								.tpriority = osPriorityAboveNormal;
						}
						tasks[i] = osThreadCreate(
							osThread(forceSwitchPriorityThread),
							task_args[i]);
						if (tasks[i] == NULL) {
							// Handle error: Failed to create
							// task
						}
					}

					counter_start(counter_dev);
					start_flag = 1;
					osDelay(10); // 10 milisecond block for main task

					counter_stop(counter_dev);

					for (size_t i = 0; i < task_count_sum; i++) // For each task
					{
						osThreadTerminate(tasks[i]);
					}

					for (size_t i = 0; i < task_count_sum; i++) // For each task
					{
						if (CodeScoreFrame(buffer_tx,
								   CMD_TASK_FORCE_SWITCH_PRIORITY,
								   (uint16_t)(args[2] * 4),
								   (uint8_t *)(values[i])) == 0) {
							uart_transmit(buffer_tx, SCORE_FRAME_SIZE,
								      1000);
						}
						// osDelay(10);
					}

				} break;

				case CMD_TASK_SWITCH: {
					resetValues(buffer_tx,
						    buffer_rx); // Reseting all values

					// Argument 1 - Number of threads
					// Argument 2 - Number of measurements per task

					uint8_t task_args[MAX_THREADS][2];
					osThreadDef2(switchThread, osPriorityNormal, MAX_THREADS,
						     defalut_stack_size * 2, stacks_shared,
						     bitarray_shared);

					for (size_t i = 0; i < args[0]; i++) {
						task_args[i][0] = i;
						task_args[i][1] = args[1];
						tasks[i] = osThreadCreate(osThread(switchThread),
									  task_args[i]);
						if (tasks[i] == NULL) {
							// Handle error: Failed to create
							// task
						}
					}

					counter_start(counter_dev);
					start_flag = 1;
					osDelay(3000); // 10 milisecond block for main task

					counter_stop(counter_dev);

					for (size_t i = 0; i < args[0]; i++) // For each task
					{
						osThreadTerminate(tasks[i]);
					}

					for (size_t i = 0; i < args[0]; i++) // For each task
					{
						if (CodeScoreFrame(buffer_tx, CMD_TASK_SWITCH,
								   (uint16_t)(args[1] * 4),
								   (uint8_t *)(values[i])) == 0) {
							uart_transmit(buffer_tx, SCORE_FRAME_SIZE,
								      1000);
						}
						// osDelay(10);
					}
				} break;

				case CMD_TASK_SWITCH_PRIORITY: {
					resetValues(buffer_tx,
						    buffer_rx); // Reseting all values

					// Argument 1 - Number of threads Low
					// Argument 2 - Number of threads High
					// Argument 3 - Number of measurements per task
					uint8_t task_count_sum = args[0] + args[1];
					uint8_t task_args[MAX_THREADS][2];

					osThreadDef2(switchPriorityThread, osPriorityBelowNormal,
						     MAX_THREADS, defalut_stack_size * 2,
						     stacks_shared, bitarray_shared);

					for (size_t i = 0; i < task_count_sum; i++) {
						// Create the task
						task_args[i][0] = i;
						task_args[i][1] = args[2];
						if (i < args[0]) {
							os_thread_def_switchPriorityThread
								.tpriority = osPriorityBelowNormal;
						} else {
							os_thread_def_switchPriorityThread
								.tpriority = osPriorityAboveNormal;
						}
						tasks[i] = osThreadCreate(
							osThread(switchPriorityThread),
							task_args[i]);
						if (tasks[i] == NULL) {
							// Handle error: Failed to create
							// task
						}
					}
					counter_start(counter_dev);
					start_flag = 1;
					osDelay(3000); // 10 milisecond block for main task

					counter_stop(counter_dev);

					for (size_t i = 0; i < task_count_sum; i++) // For each task
					{
						osThreadTerminate(tasks[i]);
					}

					for (size_t i = 0; i < task_count_sum; i++) // For each task
					{
						if (CodeScoreFrame(buffer_tx,
								   CMD_TASK_SWITCH_PRIORITY,
								   (uint16_t)(args[2] * 4),
								   (uint8_t *)(values[i])) == 0) {
							uart_transmit(buffer_tx, SCORE_FRAME_SIZE,
								      1000);
						}
						// osDelay(10);
					}
				} break;

				case CMD_SEMAPHORE: {
					resetValues(buffer_tx,
						    buffer_rx); // Reseting all values

					// Argument 1 - Number of threads
					// Argument 2 - Number of measurements per task

					uint8_t task_args[args[0]][2];
					semaphoreHandle = osSemaphoreCreate(
						osSemaphore(Semaphore),
						1); // Creating bionary semaphore (mutex)

					osThreadDef2(semaphoreThread, osPriorityNormal, MAX_THREADS,
						     defalut_stack_size * 2, stacks_shared,
						     bitarray_shared);

					for (size_t i = 0; i < args[0]; i++) {
						task_args[i][0] = i;
						task_args[i][1] = args[1];

						tasks[i] = osThreadCreate(osThread(semaphoreThread),
									  task_args[i]);

						if (tasks[i] == NULL) {
							// Handle error: Failed to create
							// task
						}
					}

					counter_start(counter_dev);
					start_flag = 1;
					osDelay(10); // 10 milisecond block for main task

					counter_stop(counter_dev);

					for (size_t i = 0; i < args[0]; i++) // For each task
					{
						osThreadTerminate(tasks[i]);
					}

					osSemaphoreDelete(semaphoreHandle);

					for (size_t i = 0; i < args[0]; i++) // For each task
					{
						if (CodeScoreFrame(buffer_tx, CMD_SEMAPHORE,
								   (uint16_t)(args[1] * 4),
								   (uint8_t *)(values[i])) == 0) {
							uart_transmit(buffer_tx, SCORE_FRAME_SIZE,
								      1000);
						}
						// osDelay(10);
					}
				} break;

				case CMD_QUEUE: {
					resetValues(buffer_tx,
						    buffer_rx); // Reseting all values

					osThreadDef2(queueRecieverThread, osPriorityNormal, 2,
						     defalut_stack_size, stacks_shared,
						     bitarray_shared);

					// Argument 1 - Number of measurements per task

					queueHandle = osMessageCreate(
						osMessageQ(Queue),
						NULL); // Creating bionary semaphore (mutex)

					os_thread_def_queueRecieverThread.pthread =
						queueTransmitterThread;
					tasks[0] =
						osThreadCreate(osThread(queueRecieverThread), args);
					if (tasks[0] == NULL) {
						// Handle error: Failed to create task
					}
					os_thread_def_queueRecieverThread.pthread =
						queueRecieverThread;
					tasks[1] =
						osThreadCreate(osThread(queueRecieverThread), args);
					if (tasks[1] == NULL) {
						// Handle error: Failed to create task
					}

					counter_start(counter_dev);
					start_flag = 1;
					osDelay(10); // 10 milisecond block for main task

					counter_stop(counter_dev);

					for (size_t i = 0; i < 2; i++) // For each task
					{
						osThreadTerminate(tasks[i]);
					}

					// osMessageDelete(queueHandle);

					for (size_t i = 0; i < 2; i++) // For each task
					{
						if (CodeScoreFrame(buffer_tx, CMD_QUEUE,
								   (uint16_t)(args[0] * 4),
								   (uint8_t *)(values[i])) == 0) {
							uart_transmit(buffer_tx, SCORE_FRAME_SIZE,
								      1000);
						}
						// osDelay(10);
					}
				} break;

				case CMD_COMMAND_NO: {
					if (CodeScoreFrame(buffer_tx, CMD_COMMAND_NO, 0, args) ==
					    0) {
						// HAL_UART_Transmit(&huart2, buffer_tx, 10,
						// 100);
					}
				} break;
				default:
					break;
				}
			}
		}
	}
}
