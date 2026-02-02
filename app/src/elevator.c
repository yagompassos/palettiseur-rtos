/*
 * elevator.c
 *
 *  Created on: Feb 2, 2026
 *      Author: Yago
 */

#include "elevator.h"
#include "main.h"

extern xSemaphoreHandle xSemElevator;
extern xQueueHandle xSubscribeQueue;


/*
 *	TaskElevator controls "Ascenseur"
 *	Receives the pallet and goes up with it
 *
*/
void vTaskElevator (void *pvParameters){
	sensor_sub_msg_t sub_pallet_arrived = {ONE_SHOT, ID_SEMAPH_ELEVATOR, SEN_ENTREE_PALETTE, 1};
	sensor_sub_msg_t sub_elevator_charged = {ONE_SHOT, ID_SEMAPH_ELEVATOR, SEN_SORTIE_PALETTE, 1};
	sensor_sub_msg_t sub_elevator_stage1 = {ONE_SHOT, ID_SEMAPH_ELEVATOR, SEN_ASCENSEUR_ETAGE_1, 1};
//	sensor_sub_msg_t sub_elevator_not_stage1 = {ONE_SHOT, ID_SEMAPH_ELEVATOR, SEN_ASCENSEUR_ETAGE_1, 0};

	while (1) {

		// Wait for pallet to arrive
		xQueueSendToBack(xSubscribeQueue, &sub_pallet_arrived, 0);
		xSemaphoreTake(xSemElevator, portMAX_DELAY);

		// Charge Elevator
		FACTORY_IO_Actuators_Modify(1, ACT_CHARGER_PALETTE);

		// Wait for elevator to be charged
		xQueueSendToBack(xSubscribeQueue, &sub_elevator_charged, 0);
		xSemaphoreTake(xSemElevator, portMAX_DELAY);

		// Stop elevator charging
		FACTORY_IO_Actuators_Modify(0, ACT_CHARGER_PALETTE);

		// Elevator UP
		FACTORY_IO_Actuators_Modify(1, ACT_ASCENSEUR_TO_LIMIT);
		FACTORY_IO_Actuators_Modify(1, ACT_MONTER_ASCENSEUR);

		// Wait elevator to go up to it's limit
		xQueueSendToBack(xSubscribeQueue, &sub_elevator_stage1, 0);
		xSemaphoreTake(xSemElevator, portMAX_DELAY);

		// Stop Elevator
		FACTORY_IO_Actuators_Modify(0, ACT_ASCENSEUR_TO_LIMIT);
		FACTORY_IO_Actuators_Modify(0, ACT_MONTER_ASCENSEUR);


		vTaskDelay(500);
	}
}
