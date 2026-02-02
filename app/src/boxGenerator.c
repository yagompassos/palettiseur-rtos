/*
 * boxGenerator.c
 *
 *  Created on: Jan 14, 2026
 *      Author: Yago
 */

#include "boxGenerator.h"
#include "main.h"

extern xSemaphoreHandle xSemBoxGenerator;

void vTaskBoxGenerator (void *pvParameters) {
	while (1) {

		// Wait before generate
		xSemaphoreTake(xSemBoxGenerator, portMAX_DELAY);

		// Has the semaphore, can generate other 2 boxes
		FACTORY_IO_Actuators_Modify(1, ACT_DISTRIBUTION_CARTONS);
		vTaskDelay(3500); // time for the 2nd box to be generated
		FACTORY_IO_Actuators_Modify(0, ACT_DISTRIBUTION_CARTONS);
		vTaskDelay(5000);
	}
}
