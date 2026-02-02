/*
 * main.c
 *
 *  Created on: 06 janvier 2026
 *      Author: Yago
 */

#include "main.h"

//Local Static Functions
static uint8_t 	SystemClock_Config	(void);
void eraseSubscription (sensor_sub_msg_t *subscription);

// FreeRTOS tasks
void vTaskRead 	(void *pvParameters);
void vTaskSTOP(void *pvParameters);
void vTaskBoxGenerator (void *pvParameters) ;

// Kernel Objects
xQueueHandle xSubscribeQueue, xWriteQueue;
xSemaphoreHandle xSemGenerator, xSemDistributor, xSemBlocker, xSemPusher;
xSemaphoreHandle xMutexConveyor;

int main(void)
{
	// Configure System Clock for 48MHz from 8MHz HSE
	SystemClock_Config();

	// Initialize LED and USER Button
	BSP_LED_Init();
	BSP_PB_Init();

	// Initialize Debug Console
	BSP_Console_Init();
	my_printf("\r\nConsole Ready!\r\n");
	my_printf("SYSCLK = %d Hz\r\n", SystemCoreClock);

	//	xTraceEnable(TRC_START);

	// Read all states from the scene
	FACTORY_IO_update();

	// Semaphores initializations
	xSemDistributor = xSemaphoreCreateBinary();
	xSemBlocker = xSemaphoreCreateBinary();
	xSemPusher = xSemaphoreCreateBinary();
	xSemGenerator = xSemaphoreCreateBinary();

	// Mutex
    xMutexConveyor = xSemaphoreCreateMutex();

	// Messague queues initialization
	xSubscribeQueue = xQueueCreate(SUBSCRIPTION_TABLE_SIZE, sizeof(sensor_sub_msg_t));
	xWriteQueue = xQueueCreate(SUBSCRIPTION_TABLE_SIZE, sizeof(actuator_cmd_msg_t));

	// Creating FreeRTOS tasks
	xTaskCreate(vTaskRead, "Task_Read", 256, NULL, 3, NULL);
	xTaskCreate(vTaskBoxGenerator, "Task_CartonGenerator", 64, NULL, 1, NULL);
	xTaskCreate(vTaskBlocker, "Task_Blocker", 128, NULL, 1, NULL);
	xTaskCreate(vTaskPusher, "Task_Pusher", 64, NULL, 1, NULL);

	// Roll every Conveyor in scene
	FACTORY_IO_Actuators_Modify(1, ACT_TAPIS_DISTRIBUTION_CARTONS);
	FACTORY_IO_Actuators_Modify(1, ACT_TAPIS_CARTON_VERS_PALETTISEUR);
	FACTORY_IO_Actuators_Modify(1, ACT_BLOCAGE_ENTREE_PALETTISEUR);
	FACTORY_IO_Actuators_Modify(1, ACT_CHARGER_PALETTISEUR);

	// Initialize with box generation
	xSemaphoreGive(xSemGenerator);

	// Start the Scheduler
	my_printf("Starting Scheduler...\r\n");
	vTaskStartScheduler();

	// Loop forever
	while(1)
	{
		// Should not be here
	}
}

void vTaskBoxGenerator (void *pvParameters) {
	while (1) {

		// Wait before generate
		xSemaphoreTake(xSemGenerator, portMAX_DELAY);

		// Has the semaphore, can generate other 2 boxes
		FACTORY_IO_Actuators_Modify(1, ACT_DISTRIBUTION_CARTONS);
		vTaskDelay(3500); // time for the 2nd box to be generated
		FACTORY_IO_Actuators_Modify(0, ACT_DISTRIBUTION_CARTONS);
		vTaskDelay(5000);
	}
}

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


void vTaskWrite (void *pvParameters) {
	sensor_sub_msg_t cmd_received;
//	static uint32_t actuator_current_mask = 0;

	while (1) {
		xQueueReceive(xWriteQueue, &cmd_received, 0);

	}
	vTaskDelay(100);
}


/*
 * stop everything in scene
 */
void vTaskSTOP (void *pvParameters) {
	while (1) {
		if (BSP_PB_GetState() == 1) {

			my_printf("stopping\r\n");
			FACTORY_IO_Actuators_Set(0);
		}
		vTaskDelay(50);
	}
}

void eraseSubscription (sensor_sub_msg_t *subscription) {
	subscription->sub_mode = 0;
	subscription->semaph_id = 0;
	subscription->sensor_id = 0;
	subscription->sensor_state = 0;
}

/*
 * 	Clock configuration for the Nucleo STM32F072RB board
 * 	HSE input Bypass Mode 			-> 8MHz
 * 	SYSCLK, AHB, APB1 				-> 48MHz
 *  PA8 as MCO with /16 prescaler 	-> 3MHz
 */
static uint8_t SystemClock_Config()
{
	uint32_t	status;
	uint32_t	timeout;

	// Start HSE in Bypass Mode
	RCC->CR |= RCC_CR_HSEBYP;
	RCC->CR |= RCC_CR_HSEON;

	// Wait until HSE is ready
	timeout = 1000;

	do
	{
		status = RCC->CR & RCC_CR_HSERDY_Msk;
		timeout--;
	} while ((status == 0) && (timeout > 0));

	if (timeout == 0) return (1);	// HSE error


	// Select HSE as PLL input source
	RCC->CFGR &= ~RCC_CFGR_PLLSRC_Msk;
	RCC->CFGR |= (0x02 <<RCC_CFGR_PLLSRC_Pos);

	// Set PLL PREDIV to /1
	RCC->CFGR2 = 0x00000000;

	// Set PLL MUL to x6
	RCC->CFGR &= ~RCC_CFGR_PLLMUL_Msk;
	RCC->CFGR |= (0x04 <<RCC_CFGR_PLLMUL_Pos);

	// Enable the main PLL
	RCC-> CR |= RCC_CR_PLLON;

	// Wait until PLL is ready
	timeout = 1000;

	do
	{
		status = RCC->CR & RCC_CR_PLLRDY_Msk;
		timeout--;
	} while ((status == 0) && (timeout > 0));

	if (timeout == 0) return (2);	// PLL error


	// Set AHB prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	//Set APB1 prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_PPRE_Msk;
	RCC->CFGR |= RCC_CFGR_PPRE_DIV1;

	// Enable FLASH Prefetch Buffer and set Flash Latency (required for high speed)
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

	// Select the main PLL as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	// Wait until PLL becomes main switch input
	timeout = 1000;

	do
	{
		status = (RCC->CFGR & RCC_CFGR_SWS_Msk);
		timeout--;
	} while ((status != RCC_CFGR_SWS_PLL) && (timeout > 0));

	if (timeout == 0) return (3);	// SW error


	// Set MCO source as SYSCLK (48MHz)
	RCC->CFGR &= ~RCC_CFGR_MCO_Msk;
	RCC->CFGR |=  RCC_CFGR_MCOSEL_SYSCLK;

	// Set MCO prescaler to /16 -> 3MHz
	RCC->CFGR &= ~RCC_CFGR_MCOPRE_Msk;
	RCC->CFGR |=  RCC_CFGR_MCOPRE_DIV16;

	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA8 as Alternate function
	GPIOA->MODER &= ~GPIO_MODER_MODER8_Msk;
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODER8_Pos);

	// Set to AF0 (MCO output)
	GPIOA->AFR[1] &= ~(0x0000000F);
	GPIOA->AFR[1] |=  (0x00000000);

	// Update SystemCoreClock global variable
	SystemCoreClockUpdate();
	return (0);
}
