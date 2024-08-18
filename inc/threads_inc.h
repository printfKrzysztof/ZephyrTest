/**
 * @file threads.h
 * @author Krzysztof Bortnowski (krzysztof.bortnowski@sesto.pl)
 * @brief
 * @version 0.1
 * @date 2024-04-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef THREADS_INC_H_
#define THREADS_INC_H_

#define MAX_THREADS         22
#define MAX_TEST_PER_THREAD 80

#include <zephyr/device.h>
#include "cmsis_os.h"
#include <zephyr/drivers/counter.h>
#include <zephyr/drivers/uart.h>

void mainThread(void const *argument);
void forceSwitchThread(void const *argument);
void switchThread(void const *argument);
void semaphoreThread(void const *argument);
void queueTransmitterThread(void const *argument);
void queueRecieverThread(void const *argument);
int uart_receive(uint8_t *buffer, size_t length, int timeout);
int uart_transmit(const uint8_t *buffer, size_t length, int timeout);
uint32_t get_time();

extern osMutexId mutexHandle;
extern osMessageQId queueHandle;

extern const struct device *counter_dev;
extern const struct device *uart2;

extern uint32_t values[MAX_THREADS][MAX_TEST_PER_THREAD];
extern int start_flag;

extern struct counter_top_cfg top_cfg;
#endif // THREADS_INC_H_
