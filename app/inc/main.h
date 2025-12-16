/*
 * main.h
 *
 *  Created on: 16 ao�t 2018
 *      Author: Laurent
 */

#ifndef APP_INC_MAIN_H_
#define APP_INC_MAIN_H_

// Device header
#include "stm32f0xx.h"
// BSP functions
#include "bsp.h"
// FreeRTOS headers
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "stream_buffer.h"


/* Global functions */

int my_printf	(const char *format, ...);
int my_sprintf	(char *out, const char *format, ...);


#endif /* APP_INC_MAIN_H_ */
