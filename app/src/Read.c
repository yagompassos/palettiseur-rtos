/*
 * Read.c
 *
 *  Created on: Feb 2, 2026
 *      Author: yago.milagres-passos
 */

#include "Read.h"
#include "main.h"

//extern kernel objets
extern xSemaphoreHandle xSemGenerator, xSemDistributor, xSemBlocker, xSemPusher, xSemElevator, xSemDoor;
extern xQueueHandle xSubscribeQueue;

void eraseSubscription (sensor_sub_msg_t *subscription);

void vTaskRead (void *pvParameters)
{
	sensor_sub_msg_t msg;
	sensor_sub_msg_t subscriptions[SUBSCRIPTION_TABLE_SIZE];
	portTickType xLastWakeTime;
	uint8_t isDuplicate = pdFALSE;
	int8_t slotAvailable;
	static uint8_t prev_sensor_state[SENSOR_TABLE_SIZE] = {0};


	xLastWakeTime = xTaskGetTickCount();

	// Earase tables
	for (int i=0; i<SUBSCRIPTION_TABLE_SIZE; i++) {
        subscriptions[i].sub_mode = 0;
		subscriptions[i].semaph_id = 0;
		subscriptions[i].sensor_id = 0;
		subscriptions[i].sensor_state = 0;
	}


	while(1){
		// Subscribe queue demand
		if (xQueueReceive(xSubscribeQueue, &msg, 0) == pdTRUE ) {
			isDuplicate = pdFALSE;
			slotAvailable = -1;

			// Verify if it's already subscribed (same sensor, same state, same semaphore)
			for (int i=0; i<SUBSCRIPTION_TABLE_SIZE; i++) {
				if (subscriptions[i].semaph_id != 0 &&
					subscriptions[i].sensor_id == msg.sensor_id &&
					subscriptions[i].sensor_state == msg.sensor_state &&
					subscriptions[i].semaph_id == msg.semaph_id) {
					isDuplicate = pdTRUE;
					break;
				}
				if (subscriptions[i].semaph_id == 0 && slotAvailable == -1) {
					slotAvailable = i;  // First available slot
				}
			}

			// Add subscription
			if (isDuplicate == pdFALSE && slotAvailable != -1) {
				subscriptions[slotAvailable] = msg;
//				my_printf("\rSubscriptions\n");
//				for (int i=0; i<SUBSCRIPTION_TABLE_SIZE; i++)
//					my_printf("\r\nSemID=%d SensID=%d State=%d\n", subscriptions[i].semaph_id, subscriptions[i].sensor_id, subscriptions[i].sensor_state);

			}
		}


		// VERIFY SUBSCRIPTION ATTENDED RESULTS
			for (int i=0; i<SUBSCRIPTION_TABLE_SIZE; i++)
			{
				if (subscriptions[i].semaph_id != 0) // verify if it's a subscription or just an empty space
				{
					uint8_t current = FACTORY_IO_Sensors_Get(subscriptions[i].sensor_id);

					if (current != prev_sensor_state[i])
					{
					    if (current == subscriptions[i].sensor_state)
					    {
							switch (subscriptions[i].semaph_id)
					    	{
					    	case ID_SEMAPH_BLOCKER:
								xSemaphoreGive(xSemBlocker);
								break;

					    	case ID_SEMAPH_PUSHER:
					    		xSemaphoreGive(xSemPusher);
								break;

							case ID_SEMAPH_ELEVATOR:
								xSemaphoreGive(xSemElevator);
								break;

							case ID_SEMAPH_DOOR:
								xSemaphoreGive(xSemDoor);
								break;
							}

					        if (subscriptions[i].sub_mode == ONE_SHOT)
					            eraseSubscription(&subscriptions[i]);
					    }

					    prev_sensor_state[i] = current;
					}

				}
			}


	vTaskDelayUntil(&xLastWakeTime, 200);
	}
}

void eraseSubscription (sensor_sub_msg_t *subscription) {
	subscription->sub_mode = 0;
	subscription->semaph_id = 0;
	subscription->sensor_id = 0;
	subscription->sensor_state = 0;
}
