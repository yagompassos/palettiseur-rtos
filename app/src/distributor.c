/*
 * distributor.c
 *
 *  Created on: Jan 14, 2026
 *      Author: yago.milagres-passos
 */

#include "distributor.h"
#include "main.h"

extern xSemaphoreHandle xSemDistributor;
extern xQueueHandle xSubscribeQueue;

 // Click at the blue button at your board, this generates new packages (cartons) to the scene.
void vTaskDistributor (void *pvParameters) {
	sensor_sub_msg_t sub_msg1 = {ONE_SHOT, ID_SEMAPH_DISTRIBUTOR, SEN_CARTON_DISTRIBUE, 1};
	sensor_sub_msg_t sub_msg2 = {ONE_SHOT, ID_SEMAPH_DISTRIBUTOR, SEN_CARTON_ENVOYE, 1};

	while(1) {
		xQueueSendToBack(xSubscribeQueue, &sub_msg1, 0);
		xSemaphoreTake(xSemDistributor, 0);

		FACTORY_IO_Actuators_Modify(1, ACT_TAPIS_DISTRIBUTION_CARTONS);
		xQueueSendToBack(xSubscribeQueue, &sub_msg2, 0);
		xSemaphoreTake(xSemDistributor, portMAX_DELAY);
		FACTORY_IO_Actuators_Modify(0, ACT_TAPIS_DISTRIBUTION_CARTONS);


	vTaskDelay(500);
	}
}
