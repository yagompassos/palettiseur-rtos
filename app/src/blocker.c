/*
 * blocker.c
 *
 *  Created on: Jan 14, 2026
 *      Author: yago.milagres-passos
 */

#include "blocker.h"
#include "factory_io.h"
#include "FreeRTOS.h"
#include "main.h"

extern xSemaphoreHandle xSemBlocker;
extern xQueueHandle xSubscribeQueue;

/*
 *	TaskControl controls "2. BLOCAGE ENTREE PALETTISEUR"
 *	It's the barrier that let the packages in to one spot before the elevator
 *
*/
void vTaskBlocker (void *pvParameters) {
	sensor_sub_msg_t sub_msg1 = {ONE_SHOT, ID_SEMAPH_BLOCKER, SEN_CARTON_ENVOYE, ACTIVE_LOW};
	sensor_sub_msg_t sub_msg2 = {ONE_SHOT, ID_SEMAPH_BLOCKER, SEN_ENTREE_PALETTISEUR, ACTIVE_LOW};

	while (1) {
		xQueueSendToBack(xSubscribeQueue, &sub_msg1, 0);
		xSemaphoreTake(xSemBlocker, portMAX_DELAY);
		FACTORY_IO_Actuators_Modify(1, ACT_TAPIS_CARTON_VERS_PALETTISEUR);
		xQueueSendToBack(xSubscribeQueue, &sub_msg2, 0);
		xSemaphoreTake(xSemBlocker, portMAX_DELAY);
		FACTORY_IO_Actuators_Modify(0, ACT_TAPIS_CARTON_VERS_PALETTISEUR);

	}
	vTaskDelay(50);
}


