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
/*
 *	TaskControl controls "2. BLOCAGE ENTREE PALETTISEUR"
 *	It's the barrier that let the packages in to one spot before the elevator
 *
 */
void vTaskControlBlocker (void *pvParameters) {
	while (1) {
		xEventGroupWaitBits(sensorsEventGroup, EVENT_2eme_CARDBOX_ENTREE_PALETTISEUR, pdFALSE, pdFALSE, portMAX_DELAY);
		my_printf("blocking\r\n");
		FACTORY_IO_Actuators_Modify(1, ACT_CHARGER_PALETTISEUR);
		FACTORY_IO_Actuators_Modify(0, ACT_BLOCAGE_ENTREE_PALETTISEUR);
		vTaskDelay(1500); // time to let two packages pass.
		FACTORY_IO_Actuators_Modify(1, ACT_BLOCAGE_ENTREE_PALETTISEUR);
	}
	vTaskDelay(50);
}
