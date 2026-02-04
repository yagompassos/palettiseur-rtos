/*
 * boxGenerator.c
 *
 *  Created on: Jan 14, 2026
 *      Author: Yago
 */

#include "boxGenerator.h"
#include "main.h"

extern xTaskHandle		vTaskBoxGenerator_handle;
extern xQueueHandle 	xWriteQueue;

void vTaskBoxGenerator (void *pvParameters) {
	actuator_cmd_msg_t cmd_generate_box = {ACT_DISTRIBUTION_CARTONS , 1};
	actuator_cmd_msg_t cmd_stop = {ACT_DISTRIBUTION_CARTONS , 0};

	while (1) {

		// Wait notify
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// Has the semaphore, can generate other 2 boxes
		xQueueSendToBack(xWriteQueue, &cmd_generate_box, 0);
		vTaskDelay(4000); // time for the 2nd box to be generated
		xQueueSendToBack(xWriteQueue, &cmd_stop, 0);
	}
}
