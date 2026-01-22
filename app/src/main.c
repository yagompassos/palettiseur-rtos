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
void vTaskSensorMonitor(void *pvParameters);
void vTaskControlCartons (void *pvParameters);
void vTaskSTOP(void *pvParameters);

// Kernel Objects
EventGroupHandle_t sensorsEventGroup = NULL;
xQueueHandle xSubscribeQueue, xWriteQueue;
xSemaphoreHandle xSem1, xSem2, xSemCartons;

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

	/*
	 *  right now tracealyzer doesn't show much info... so its better to leave it like this
	 *  This means TraceFacility its also deactivated in FreeRTOSConfig.h
	 *  The program doesnt run if trace is activated but you havent opened Percepio Tracealyzer (so we better leave all commented right now)
	 */
	//	xTraceEnable(TRC_START);    // <====== We uncomment that when we want to see the results in tracealyzer

	// Read all states from the scene
	FACTORY_IO_update();

	// Create Event Group
	xSemCartons = xSemaphoreCreateBinary();
	xSem2 = xSemaphoreCreateBinary();
	sensorsEventGroup = xEventGroupCreate();
	xSubscribeQueue = xQueueCreate(8, sizeof(sensor_sub_msg_t));
	xWriteQueue = xQueueCreate(8, sizeof(actuator_cmd_msg_t));

	// Register Trace events
	// ue1 = xTraceRegisterString("state");

	// Creating FreeRTOS tasks
//	xTaskCreate(vTaskSensorMonitor, "Task_Sensor_Monitor", 256, NULL, 3, NULL);
	xTaskCreate(vTaskRead, "Task_Read", 256, NULL, 3, NULL);
	xTaskCreate(vTaskControlCartons, "Task_Control_Cartons", 128, NULL, 1, NULL);
	//	xTaskCreate(vTaskControlBlocker, "Task_Control_Blocker", 128, NULL, 2, NULL);
//	xTaskCreate(vTaskDistribuitionCardBoards, "Task_DistribuitionCardBoards", 256, NULL, 2, NULL);

	//FACTORY_IO_Actuators_Modify(1, ACT_TAPIS_DISTRIBUTION_CARTONS);
	FACTORY_IO_Actuators_Modify(1, ACT_TAPIS_CARTON_VERS_PALETTISEUR);
	FACTORY_IO_Actuators_Modify(1, ACT_BLOCAGE_ENTREE_PALETTISEUR);

	// Start the Scheduler
	my_printf("Starting Scheduler...\r\n");
	vTaskStartScheduler();

	// Loop forever
	while(1)
	{
		// Should not be here
	}
}

// Task Sensor Monitor receives value of all scene sensors and notifies tasks.
void vTaskSensorMonitor (void *pvParameters){
    while(1){
    	// DEVE SE LER O factoryIO COM update TODA VEZ ANTES DE PUXAR O GET, CHECAR EMAILS.
//    	FACTORY_IO_update();

    	if (FACTORY_IO_Sensors_Get(SEN_CARTON_DISTRIBUE))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_CARTON_DISTRIBUE);
		else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_CARTON_DISTRIBUE);

        if (FACTORY_IO_Sensors_Get(SEN_CARTON_ENVOYE))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_CARTON_ENVOYE);
        else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_CARTON_ENVOYE);

        if (FACTORY_IO_Sensors_Get(SEN_ENTREE_PALETTISEUR))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_ENTREE_PALETTISEUR);
        else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_ENTREE_PALETTISEUR);

    	if (FACTORY_IO_Sensors_Get(SEN_PORTE_OUVERTE))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_PORTE_OUVERTE);
		else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_PORTE_OUVERTE);

        if (FACTORY_IO_Sensors_Get(SEN_LIMITE_POUSSOIR))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_LIMITE_POUSSOIR);
        else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_LIMITE_POUSSOIR);

        if (FACTORY_IO_Sensors_Get(SEN_CLAMPED))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_CLAMPED);
		else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_CLAMPED);

    	if (FACTORY_IO_Sensors_Get(SEN_ASCENSEUR_ETAGE_RDC))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_ASCENSEUR_ETAGE_RDC);
		else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_ASCENSEUR_ETAGE_RDC);

        if (FACTORY_IO_Sensors_Get(SEN_ASCENSEUR_ETAGE_1))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_ASCENSEUR_ETAGE_1);
        else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_ASCENSEUR_ETAGE_1);

        if (FACTORY_IO_Sensors_Get(SEN_ASCENSEUR_ETAGE_2))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_ASCENSEUR_ETAGE_2);
		else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_ASCENSEUR_ETAGE_2);

        if (FACTORY_IO_Sensors_Get(SEN_SORTIE_PALETTE))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_SORTIE_PALETTE);
        else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_SORTIE_PALETTE);

        if (FACTORY_IO_Sensors_Get(SEN_LIMITE_PORTE))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_LIMITE_PORTE);
		else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_LIMITE_PORTE);

    	if (FACTORY_IO_Sensors_Get(SEN_ASCENSEUR_EN_MOUVEMENT))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_ASCENSEUR_EN_MOUVEMENT);
		else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_ASCENSEUR_EN_MOUVEMENT);

        if (FACTORY_IO_Sensors_Get(SEN_ENTREE_PALETTE))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_ENTREE_PALETTE);
        else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_ENTREE_PALETTE);

        if (FACTORY_IO_Sensors_Get(SEN_BUTEE_CARTON))
			xEventGroupSetBits(sensorsEventGroup, EVENT_SEN_BUTEE_CARTON);
		else
			xEventGroupClearBits(sensorsEventGroup, EVENT_SEN_BUTEE_CARTON);

        vTaskDelay(20);
    }
}

void vTaskRead (void *pvParameters)
{
	sensor_sub_msg_t msg;
	sensor_sub_msg_t subscriptions[SUBSCRIPTION_TABLE_SIZE];
	uint8_t sensor[SENSOR_TABLE_SIZE];
	portTickType xLastWakeTime;
	uint8_t isDuplicate;
	int8_t slotAvailable;
	uint8_t rx_byte;

	xLastWakeTime = xTaskGetTickCount();

	// Earase tables
	for (int i=0; i<SUBSCRIPTION_TABLE_SIZE; i++){
		subscriptions[i].semaph_id = 0;
		subscriptions[i].sensor_id = 0;
		subscriptions[i].sensor_state = 0;
	}
	for (int i=0; i<SENSOR_TABLE_SIZE; i++)
		sensor[i] = 0;

	while(1){
		// Subscribe queue demand
		if (xQueueReceive(xSubscribeQueue, &msg, 0) == pdTRUE ) {
			my_printf("\r\n[Publisher] Subscribing : SemID=%d SensID=%d State=%d\n", msg.semaph_id, msg.sensor_id, msg.sensor_state);

			slotAvailable = -1;

			// Verify if its already subscribed
			for (int i=0; i<SUBSCRIPTION_TABLE_SIZE; i++)
			{
				if ((msg.semaph_id == subscriptions[i].semaph_id) && (msg.sensor_id == subscriptions[i].sensor_id) && (msg.sensor_state == subscriptions[i].sensor_state))
					isDuplicate = pdTRUE;
				if (subscriptions[i].semaph_id == 0)
					slotAvailable = i;
			}

			// Add subscription
			if (isDuplicate == pdFALSE && slotAvailable != -1) {
				my_printf("\r[Publisher] Adding subscription to slot [%d]\n", slotAvailable);
				subscriptions[slotAvailable] = msg;
			}
		}


		// USART KEYBOARD INTERRUPTIONS
//		if ( (USART2->ISR & USART_ISR_RXNE) == USART_ISR_RXNE ) {
//			rx_byte = USART2->RDR;
//			switch(rx_byte){
//			case ('a'):
//				sensor[0] = 0;
//				break;
//			case ('b'):
//				sensor[0] = 1;
//				break;
//			case ('c'):
//				sensor[1] = 0;
//				break;
//			case ('d'):
//				sensor[1] = 1;
//				break;
//			default:
//				break;
//			}

//			my_printf("\r\n====SENSORS====\n");
//			for (int i=0; i<SENSOR_TABLE_SIZE; i++)
//				my_printf("\r[%d] %d\n", i, sensor[i]);

		// VERIFY SUBSCRIPTION ATTENDED RESULTS
			for (int i=0; i<SUBSCRIPTION_TABLE_SIZE; i++)
			{
				if (subscriptions[i].semaph_id != 0) // verify if it's a subscription or just an empty space
				{
					if (FACTORY_IO_Sensors_Get(subscriptions[i].sensor_id) == subscriptions[i].sensor_state)
					{
						switch (subscriptions[i].semaph_id)
						{
							case ID_SEMAPH_CARTON:
								xSemaphoreGive(xSemCartons);
								break;

							default:
								break;
						}

						if (subscriptions[i].sub_mode == ONE_SHOT)
							eraseSubscription(&subscriptions[i]);
					}
				}
			}

		/*// VERIFY SUBSCRIPTION ATTENDED RESULTS
		for (int i=0; i<SUBSCRIPTION_TABLE_SIZE; i++)
		{
			if (subscriptions[i].semaph_id != 0) // verify if it's a subscription or just an empty space
			{
				for (int j=0; j<SENSOR_TABLE_SIZE; j++) {
					if (subscriptions[i].sensor_id == j+1 && subscriptions[i].sensor_state == sensor[j]) {
						my_printf("\rGiving Semaphore #%d\n", subscriptions[i].semaph_id);
						if (subscriptions[i].semaph_id == 1)
							xSemaphoreGive(xSem1);
						else if (subscriptions[i].semaph_id == 2)
							xSemaphoreGive(xSem2);
						subscriptions[i].semaph_id	= 0;
						subscriptions[i].sensor_id	= 0;
						subscriptions[i].sensor_state = 0;
						for (int i=0; i<SUBSCRIPTION_TABLE_SIZE; i++)
							my_printf("\r[%d] %d %d %d\n", i, subscriptions[i].semaph_id, subscriptions[i].sensor_id, subscriptions[i].sensor_state);
					}
				}
			}
		}
		*/

	vTaskDelayUntil(&xLastWakeTime, 200);
	//vTaskDelay(400);
	}
}

void vTaskControlCartons (void *pvParameters) {
	sensor_sub_msg_t sub_msg1 = {ONE_SHOT, ID_SEMAPH_CARTON, SEN_CARTON_DISTRIBUE, ACTIVE_LOW};
	sensor_sub_msg_t sub_msg2 = {ONE_SHOT, ID_SEMAPH_CARTON, SEN_CARTON_DISTRIBUE, IDLE_LOW};

	while(1) {
		xQueueSendToBack(xSubscribeQueue, &sub_msg1, 0);
		my_printf("[tache cartons] enviando mensagem: SemID=%d SensID=%d State=%d\n", sub_msg1.semaph_id, sub_msg1.sensor_id, sub_msg1.sensor_state);
		xSemaphoreTake(xSemCartons, portMAX_DELAY);
		my_printf("[tache cartons] RECEBI\r\n");
		FACTORY_IO_Actuators_Modify(ACTIVE_HIGH, ACT_TAPIS_DISTRIBUTION_CARTONS);

		xQueueSendToBack(xSubscribeQueue, &sub_msg2, 0);
		my_printf("[tache cartons] enviando mensagem: SemID=%d SensID=%d State=%d\n", sub_msg2.semaph_id, sub_msg2.sensor_id, sub_msg2.sensor_state);
		xSemaphoreTake(xSemCartons, portMAX_DELAY);
		my_printf("[tache cartons] RECEBI\r\n");
		FACTORY_IO_Actuators_Modify(IDLE_HIGH, ACT_TAPIS_DISTRIBUTION_CARTONS);

	}
	vTaskDelay(1000);
}


void vTaskWrite (void *pvParameters) {
	sensor_sub_msg_t cmd_received;
	static uint32_t actuator_current_mask = 0;

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
