/*
 * blocker.c
 *
 *  Created on: Jan 14, 2026
 *      Author: yago.milagres-passos
 */

#include "blocker.h"
#include "main.h"

extern xSemaphoreHandle xSemBlocker, xSemGenerator;
extern xQueueHandle xSubscribeQueue;

/*
 *	TaskBlocker controls "BLOCAGE ENTREE PALETTISEUR"
 *	It's the barrier that let the packages in to one spot before the elevator
 *
*/
void vTaskBlocker (void *pvParameters) {
	sensor_sub_msg_t sub_entree_detecte = {ONE_SHOT, ID_SEMAPH_BLOCKER, SEN_ENTREE_PALETTISEUR, 1};
	sensor_sub_msg_t sub_entree_passe = {ONE_SHOT, ID_SEMAPH_BLOCKER, SEN_ENTREE_PALETTISEUR, 0};

	while (1) {
		// Wait for first box to enter the sensor reading area
		xQueueSendToBack(xSubscribeQueue, &sub_entree_detecte, 0);
		xSemaphoreTake(xSemBlocker, portMAX_DELAY);

		// Wait the box to leave the sensor reading area
		xQueueSendToBack(xSubscribeQueue, &sub_entree_passe, 0);
		xSemaphoreTake(xSemBlocker, portMAX_DELAY);

		// Wait for SECOND box to enter the sensor reading area
		xQueueSendToBack(xSubscribeQueue, &sub_entree_detecte, 0);
		xSemaphoreTake(xSemBlocker, portMAX_DELAY);

		// Wait the box to leave the sensor reading area
		xQueueSendToBack(xSubscribeQueue, &sub_entree_passe, 0);
		xSemaphoreTake(xSemBlocker, portMAX_DELAY);

		// Send more boxes
		xSemaphoreGive(xSemGenerator);

		// Open palletizer
		FACTORY_IO_Actuators_Modify(0, ACT_BLOCAGE_ENTREE_PALETTISEUR);
		vTaskDelay(1500);
		FACTORY_IO_Actuators_Modify(1, ACT_BLOCAGE_ENTREE_PALETTISEUR);

		vTaskDelay(500);
	}
}


