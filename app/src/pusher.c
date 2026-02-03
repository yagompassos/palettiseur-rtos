/*
 * poussoir.c
 *
 *  Created on: Feb 2, 2026
 *      Author: Yago
 */


#include "pusher.h"
#include "main.h"

extern xSemaphoreHandle xSemPusher;
extern xQueueHandle 	xSubscribeQueue;
extern xTaskHandle		vTaskDoor_handle;


/*
 *	TaskPusher controls "Poussoir"
 *	When triggered, it pushes the boxes to the palletizer. It counts 3 pushes and sends a binary to Door Control
 *
*/
void vTaskPusher(void *pvParameters) {
	sensor_sub_msg_t sub_box_arrived = {ONE_SHOT, ID_SEMAPH_PUSHER, SEN_BUTEE_CARTON, 1};
	sensor_sub_msg_t sub_pusher_limit = {ONE_SHOT, ID_SEMAPH_PUSHER, SEN_LIMITE_POUSSOIR, 1};

	uint8_t pushes = 0;

	while (1) {

		// Wait for boxes to arrive
		xQueueSendToBack(xSubscribeQueue, &sub_box_arrived, 0);
		xSemaphoreTake(xSemPusher, portMAX_DELAY);

		// Push boxes
		FACTORY_IO_Actuators_Modify(1, ACT_POUSSOIR);

		// Wait for pusher achieve it's limit
		xQueueSendToBack(xSubscribeQueue, &sub_pusher_limit, 0);
		xSemaphoreTake(xSemPusher, portMAX_DELAY);

		// Retrieve pusher
		FACTORY_IO_Actuators_Modify(0, ACT_POUSSOIR);

		pushes++;

		if (pushes == THREE_PUSHES) {

			// Redefine pushing counter
			pushes = 0;
			xTaskNotifyGive(vTaskDoor_handle);

		}

		vTaskDelay(500);
	}
}


