/*
 * door.c
 *
 *  Created on: Feb 3, 2026
 *      Author: Yago
 */



#include "door.h"
#include "main.h"

extern xTaskHandle		vTaskDoor_handle;
extern xSemaphoreHandle xSemDoor, xSemElevator;
extern xQueueHandle 	xSubscribeQueue;


/*
 *	TaskDoor controls "Door" and "Clamper"
 *	When triggered, it clamps the boxes inside the palletizer together
 *	and opens the door so the boxes go to the pallet in the elevator
 *
*/
void vTaskDoor(void *pvParameters) {
	sensor_sub_msg_t sub_clamped = {ONE_SHOT, ID_SEMAPH_DOOR, SEN_CLAMPED, 1};
	sensor_sub_msg_t sub_door_opened = {ONE_SHOT, ID_SEMAPH_DOOR, SEN_LIMITE_PORTE, 1};

	while (1) {

		// Only starts after notified by Pusher task
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// clamp
		FACTORY_IO_Actuators_Modify(1, ACT_CLAMP);

		// Wait for clamp limit
		xQueueSendToBack(xSubscribeQueue, &sub_clamped, 0);
		xSemaphoreTake(xSemDoor, portMAX_DELAY);

		// Open door
		FACTORY_IO_Actuators_Modify(1, ACT_PORTE);

		// Wait for door limit
		xQueueSendToBack(xSubscribeQueue, &sub_door_opened, 0);
		xSemaphoreTake(xSemDoor, portMAX_DELAY);

		// unclamp
		FACTORY_IO_Actuators_Modify(0, ACT_CLAMP);

		// Tell elevator to go down
		xSemaphoreGive(xSemElevator);

		vTaskDelay(500);
		// Close door
		FACTORY_IO_Actuators_Modify(0, ACT_PORTE);

		vTaskDelay(500);
	}
}


