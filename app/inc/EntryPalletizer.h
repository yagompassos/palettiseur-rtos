/*
 * blocker.h
 *
 *  Created on: Jan 14, 2026
 *      Author: Yago
 */

#ifndef INC_ENTRYPALLETIZER_H_
#define INC_ENTRYPALLETIZER_H_


/*
 *	TaskControlBlocker controls "2. BLOCAGE ENTREE PALETTISEUR"
 *	It's the barrier that let the packages in to one spot before the elevator
 *
 */
void vTaskEntryPalletizer (void *pvParameters);

#endif /* INC_ENTRYPALLETIZER_H_ */
