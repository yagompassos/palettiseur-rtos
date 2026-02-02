/*
 * blocker.h
 *
 *  Created on: Jan 14, 2026
 *      Author: Yago
 */

#ifndef INC_BLOCKER_H_
#define INC_BLOCKER_H_


/*
 *	TaskControlBlocker controls "2. BLOCAGE ENTREE PALETTISEUR"
 *	It's the barrier that let the packages in to one spot before the elevator
 *
 */
void vTaskBlocker (void *pvParameters);

#endif /* INC_BLOCKER_H_ */
