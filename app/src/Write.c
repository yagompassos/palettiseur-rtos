/*
 * Write.c
 *
 *  Created on: Feb 2, 2026
 *      Author: yago.milagres-passos
 */

#include "Write.h"
#include "main.h"

extern xQueueHandle xWriteQueue;

void vTaskWrite (void *pvParameters) {
	sensor_sub_msg_t cmd_received;
//	static uint32_t actuator_current_mask = 0;

	while (1) {
		xQueueReceive(xWriteQueue, &cmd_received, 0);

	}
	vTaskDelay(100);
}
