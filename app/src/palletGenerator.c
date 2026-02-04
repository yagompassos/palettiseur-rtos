/*
 * palletGenerator.c
 *
 *  Created on: Feb 2, 2026
 *      Author: yago.milagres-passos
 */

#include "palletGenerator.h"
#include "main.h"

extern xTaskHandle		vTaskPalletGenerator_handle;
extern xQueueHandle 	xWriteQueue;


void vTaskPalletGenerator ( void *pvParameters) {
	actuator_cmd_msg_t cmd_generate_pallet = {ACT_DISTRIBUTION_PALETTE , 1};
	actuator_cmd_msg_t cmd_stop = {ACT_DISTRIBUTION_PALETTE , 0};

	while (1) {

		// Wait notify
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// Generate 1 Pallet
		xQueueSendToBack(xWriteQueue, &cmd_generate_pallet, 0);
		vTaskDelay(500);
		xQueueSendToBack(xWriteQueue, &cmd_stop, 0);
	}
}
