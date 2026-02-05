/*
 * main.c
 *
 *  Created on: 06 janvier 2026
 *      Author: Yago
 */

#include "main.h"

// Kernel Objects
xQueueHandle 		xSubscribeQueue, xWriteQueue;
xSemaphoreHandle 	xSemBlocker, xSemPusher, xSemElevator, xSemDoor;
xSemaphoreHandle 	xBridgeMutex;
xTaskHandle			vTaskDoor_handle, vTaskBoxGenerator_handle, vTaskPalletGenerator_handle;

//Local Static Functions
static uint8_t 	SystemClock_Config	(void);

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

	// Start Trace Recording
	xTraceEnable(TRC_START);

	// Read all states from the scene
	FACTORY_IO_update();

	// Roll every Conveyor in scene
	FACTORY_IO_Actuators_Set(ALLWAYS_RUNNING_CONVEYORS);

	// Semaphores initializations
	xSemBlocker = xSemaphoreCreateBinary();
	xSemPusher = xSemaphoreCreateBinary();
	xSemElevator = xSemaphoreCreateBinary();
	xSemDoor = xSemaphoreCreateBinary();

	// Mutex for Uart
	xBridgeMutex = xSemaphoreCreateMutex();

	// Messague queues initialization
	xSubscribeQueue = xQueueCreate(SUBSCRIPTION_TABLE_SIZE, sizeof(sensor_sub_msg_t));
	xWriteQueue = xQueueCreate(COMMAND_QUEUE_SIZE, sizeof(actuator_cmd_msg_t));

	// Tracing Kernel objects:
	vTraceSetSemaphoreName(xSemBlocker, "Blocker Semaphore");
	vTraceSetSemaphoreName(xSemPusher, "Pusher Semaphore");
	vTraceSetSemaphoreName(xSemElevator, "Elevator Semaphore");
	vTraceSetSemaphoreName(xSemDoor, "Door Semaphore");
	vTraceSetMutexName(xBridgeMutex, "Bridge Mutex");
	vTraceSetQueueName(xSubscribeQueue, "Subscribe Queue");
	vTraceSetQueueName(xWriteQueue, "Write Queue");

	// Creating FreeRTOS tasks
	xTaskCreate(vTaskRead, "Task_Read", 96, NULL, 3, NULL);
	xTaskCreate(vTaskWrite, "Task_Write", 128, NULL, 2, NULL);
	xTaskCreate(vTaskBlocker, "Task_Blocker", 160, NULL, 1, NULL);
	xTaskCreate(vTaskPusher, "Task_Pusher", 104, NULL, 1, NULL);
	xTaskCreate(vTaskElevator, "Task_Elevator", 268, NULL, 1, NULL);
	xTaskCreate(vTaskDoor, "Task_Door", 144, NULL, 1, &vTaskDoor_handle);
	xTaskCreate(vTaskBoxGenerator, "Task_BoxGenerator", 96, NULL, 1, &vTaskBoxGenerator_handle);
	xTaskCreate(vTaskPalletGenerator, "Task_PalletGenerator", 96, NULL, 1, &vTaskPalletGenerator_handle);

	// Start the Scheduler
	my_printf("Starting Scheduler...\r\n");
	vTaskStartScheduler();

	// Loop forever
	while(1)
	{
		// Should not be here
	}
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
