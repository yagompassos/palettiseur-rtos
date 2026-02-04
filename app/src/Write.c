/*
 * Write.c
 *
 *  Created on: Feb 2, 2026
 *      Author: yago.milagres-passos
 */

#include "Write.h"
#include "main.h"

extern xSemaphoreHandle xBridgeMutex;
extern xQueueHandle xWriteQueue;

void vTaskWrite (void *pvParameters) {
	actuator_cmd_msg_t cmd_received;

	// Considering all this actuators will be activated allways, since the begining.
	static uint32_t actuator_current_mask = ACT_TAPIS_DISTRIBUTION_CARTONS | ACT_TAPIS_CARTON_VERS_PALETTISEUR | ACT_BLOCAGE_ENTREE_PALETTISEUR | ACT_CHARGER_PALETTISEUR
			| ACT_TAPIS_PALETTE_VERS_ASCENSEUR | ACT_TAPIS_DISTRIBUTION_PALETTE | ACT_TAPIS_FIN | ACT_REMOVER;

	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();


	while (1) {
		if (xQueueReceive(xWriteQueue, &cmd_received, pdMS_TO_TICKS(100)) == pdTRUE)
		{

			if (cmd_received.actuator_state) // Actor to 1
				actuator_current_mask |= cmd_received.actuator_mask; // bits = bits or mask
			else 							// Actor to 0
				actuator_current_mask &= ~cmd_received.actuator_mask; // bits = bits and not mask

			xSemaphoreTake(xBridgeMutex, portMAX_DELAY);
			FACTORY_IO_Actuators_Set(actuator_current_mask);
			xSemaphoreGive(xBridgeMutex);

		}
			vTaskDelayUntil(&xLastWakeTime, 50);
	}
}
