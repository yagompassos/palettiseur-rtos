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
void vTaskBlocker (void *pvParameters) {
	while (1) {

	}
	vTaskDelay(50);
}


