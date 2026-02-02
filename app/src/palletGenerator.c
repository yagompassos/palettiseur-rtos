/*
 * palletGenerator.c
 *
 *  Created on: Feb 2, 2026
 *      Author: yago.milagres-passos
 */

#include "palletGenerator.h"
#include "main.h"

extern xSemaphoreHandle xSemPalletGenerator;

void vTaskPalletGenerator ( void *pvParameters) {
	while (1) {

		// Wait for semaphore
		xSemaphoreTake(xSemPalletGenerator, portMAX_DELAY);

		// Generate 1 Pallet
		FACTORY_IO_Actuators_Modify(1, ACT_DISTRIBUTION_PALETTE);
		vTaskDelay(500);
		FACTORY_IO_Actuators_Modify(0, ACT_DISTRIBUTION_PALETTE);

		vTaskDelay(10000);
	}

}
