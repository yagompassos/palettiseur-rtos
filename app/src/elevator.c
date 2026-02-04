/*
 * elevator.c
 *
 *  Created on: Feb 2, 2026
 *      Author: Yago
 */

#include "elevator.h"
#include "main.h"

extern xTaskHandle 		vTaskPalletGenerator_handle;
extern xSemaphoreHandle xSemElevator;
extern xQueueHandle 	xSubscribeQueue, xWriteQueue;


/*
 *	TaskElevator controls "Ascenseur"
 *	Receives the pallet and goes up with it
 *
*/
void vTaskElevator (void *pvParameters){
	sensor_sub_msg_t sub_pallet_arrived = {ONE_SHOT, ID_SEMAPH_ELEVATOR, SEN_ENTREE_PALETTE, 1};
	sensor_sub_msg_t sub_elevator_charged = {ONE_SHOT, ID_SEMAPH_ELEVATOR, SEN_SORTIE_PALETTE, 1};
	sensor_sub_msg_t sub_elevator_stage1 = {ONE_SHOT, ID_SEMAPH_ELEVATOR, SEN_ASCENSEUR_ETAGE_1, 1};
	sensor_sub_msg_t sub_elevator_ground= {ONE_SHOT, ID_SEMAPH_ELEVATOR, SEN_ASCENSEUR_ETAGE_RDC, 1};
	sensor_sub_msg_t sub_pallet_gone= {ONE_SHOT, ID_SEMAPH_ELEVATOR, SEN_SORTIE_PALETTE, 0};
	actuator_cmd_msg_t cmd_charger_pallet = {ACT_CHARGER_PALETTE , 1};
	actuator_cmd_msg_t cmd_stop_charge = {ACT_CHARGER_PALETTE , 0};
	actuator_cmd_msg_t cmd_elevator_up = {ACT_MONTER_ASCENSEUR | ACT_ASCENSEUR_TO_LIMIT , 1};
	actuator_cmd_msg_t cmd_elevator_down = {ACT_DESCENDRE_ASCENSEUR, 1};
	actuator_cmd_msg_t cmd_elevator_stop = {ACT_MONTER_ASCENSEUR | ACT_DESCENDRE_ASCENSEUR | ACT_ASCENSEUR_TO_LIMIT , 0};
	actuator_cmd_msg_t cmd_elevator_to_limit = {ACT_ASCENSEUR_TO_LIMIT, 1};

	xTaskNotifyGive(vTaskPalletGenerator_handle);

	while (1) {

		// Wait for pallet to arrive
		xQueueSendToBack(xSubscribeQueue, &sub_pallet_arrived, 0);
		xSemaphoreTake(xSemElevator, portMAX_DELAY);

		// Charge Elevator
		xQueueSendToBack(xWriteQueue, &cmd_charger_pallet, 0);

		// Wait for elevator to be charged
		xQueueSendToBack(xSubscribeQueue, &sub_elevator_charged, 0);
		xSemaphoreTake(xSemElevator, portMAX_DELAY);

		// Stop elevator charging
		xQueueSendToBack(xWriteQueue, &cmd_stop_charge, 0);

		// Elevator UP
		xQueueSendToBack(xWriteQueue, &cmd_elevator_up, 0);

		// Wait elevator to go up to it's limit
		xQueueSendToBack(xSubscribeQueue, &sub_elevator_stage1, 0);
		xSemaphoreTake(xSemElevator, portMAX_DELAY);

		// Stop Elevator
		xQueueSendToBack(xWriteQueue, &cmd_elevator_stop, 0);

		// Wait first boxes
		xSemaphoreTake(xSemElevator, portMAX_DELAY);

		// Go down 1 floor
		xQueueSendToBack(xWriteQueue, &cmd_elevator_down, 0);

		// Wait second set of boxes
		xSemaphoreTake(xSemElevator, portMAX_DELAY);

		// Elevator Down
		xQueueSendToBack(xWriteQueue, &cmd_elevator_to_limit, 0);


		// Wait elevator all the way down to it's limit
		xQueueSendToBack(xSubscribeQueue, &sub_elevator_ground, 0);
		xSemaphoreTake(xSemElevator, portMAX_DELAY);

		// Stop Elevator
		xQueueSendToBack(xWriteQueue, &cmd_elevator_stop, 0);

		// Decharge Pallet
		xQueueSendToBack(xWriteQueue, &cmd_charger_pallet, 0);

		// Wait pallet to be gone off the elevator
		xQueueSendToBack(xSubscribeQueue, &sub_pallet_gone, 0);
		xSemaphoreTake(xSemElevator, portMAX_DELAY);

		// Allow new pallet to come
		xTaskNotifyGive(vTaskPalletGenerator_handle);

		vTaskDelay(500);
	}
}
