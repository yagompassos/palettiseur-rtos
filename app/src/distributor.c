/*
 * distributor.c
 *
 *  Created on: Jan 14, 2026
 *      Author: yago.milagres-passos
 */

#include "blocker.h"
#include "factory_io.h"
#include "FreeRTOS.h"
#include "main.h"


/*
 * Click at the blue button at your board, this generates new packages (cartons) to the scene.
 */
void vTaskDistribuitionCardBoards (void *pvParameters) {
	while (1) {
		xEventGroupWaitBits(sensorsEventGroup, EVENT_SEN_ENTREE_PALETTISEUR, pdFALSE, pdFALSE, portMAX_DELAY);
		FACTORY_IO_Actuators_Modify(1, ACT_DISTRIBUTION_CARTONS);
		vTaskDelay(100);
		FACTORY_IO_Actuators_Modify(0, ACT_DISTRIBUTION_CARTONS);
	}
	vTaskDelay(50);
}
