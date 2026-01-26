/*
 * blocker.c
 *
 *  Created on: Jan 14, 2026
 *      Author: yago.milagres-passos
 */

#include "EntryPalletizer.h"
#include "factory_io.h"
#include "FreeRTOS.h"
#include "main.h"

extern xSemaphoreHandle xSemBlocker, xMutexConveyor;
extern xQueueHandle xSubscribeQueue;

/*
 *	TaskControl controls "2. BLOCAGE ENTREE PALETTISEUR"
 *	It's the barrier that let the packages in to one spot before the elevator
 *
*/
void vTaskEntryPalletizer (void *pvParameters) {
	sensor_sub_msg_t sub_msg1 = {ONE_SHOT, ID_SEMAPH_BLOCKER, SEN_ENTREE_PALETTISEUR, 1};
	sensor_sub_msg_t sub_msg2 = {ONE_SHOT, ID_SEMAPH_BLOCKER, SEN_ENTREE_PALETTISEUR, 0};

	uint8_t zone_conveyor = 2;

	while (1) {
		xQueueSendToBack(xSubscribeQueue, &sub_msg1, 0);
		xSemaphoreTake(xSemBlocker, portMAX_DELAY);
		zone_conveyor--;
		my_printf("\r[task]zone_conveyor=%d\n", zone_conveyor);

		xQueueSendToBack(xSubscribeQueue, &sub_msg2, 0);
		xSemaphoreTake(xSemBlocker, portMAX_DELAY);
		xQueueSendToBack(xSubscribeQueue, &sub_msg1, 0);
		xSemaphoreTake(xSemBlocker, portMAX_DELAY);

		if (zone_conveyor <= 0) {
			// Open palletizer
			FACTORY_IO_Actuators_Modify(0, ACT_BLOCAGE_ENTREE_PALETTISEUR);
			vTaskDelay(1000);
			FACTORY_IO_Actuators_Modify(1, ACT_BLOCAGE_ENTREE_PALETTISEUR);
			zone_conveyor = 2;
		}


	vTaskDelay(500);
	}
}


