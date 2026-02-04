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
extern xQueueHandle 	xSubscribeQueue, xWriteQueue;


/*
 *	TaskDoor controls "Door" and "Clamper"
 *	When triggered, it clamps the boxes inside the palletizer together
 *	and opens the door so the boxes go to the pallet in the elevator
 *
*/
void vTaskDoor(void *pvParameters) {
	sensor_sub_msg_t sub_clamped = {ONE_SHOT, ID_SEMAPH_DOOR, SEN_CLAMPED, 1};
	sensor_sub_msg_t sub_door_opened = {ONE_SHOT, ID_SEMAPH_DOOR, SEN_LIMITE_PORTE, 1};
	actuator_cmd_msg_t cmd_clamp = {ACT_CLAMP , 1};
	actuator_cmd_msg_t cmd_unclamp = {ACT_CLAMP , 0};
	actuator_cmd_msg_t cmd_open_door = {ACT_PORTE , 1};
	actuator_cmd_msg_t cmd_close_door = {ACT_PORTE , 0};

	while (1) {

		// Only starts after notified by Pusher task
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// clamp
		xQueueSendToBack(xWriteQueue, &cmd_clamp, 0);

		// Wait for clamp limit
		xQueueSendToBack(xSubscribeQueue, &sub_clamped, 0);
		xSemaphoreTake(xSemDoor, portMAX_DELAY);

		// Open door
		xQueueSendToBack(xWriteQueue, &cmd_open_door, 0);

		// Wait for door limit
		xQueueSendToBack(xSubscribeQueue, &sub_door_opened, 0);
		xSemaphoreTake(xSemDoor, portMAX_DELAY);

		// unclamp
		xQueueSendToBack(xWriteQueue, &cmd_unclamp, 0);

		// Tell elevator to go down
		xSemaphoreGive(xSemElevator);

		vTaskDelay(500);
		// Close door
		xQueueSendToBack(xWriteQueue, &cmd_close_door, 0);

		vTaskDelay(500);
	}
}


