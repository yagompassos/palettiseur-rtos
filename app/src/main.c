/*
 * main.c
 *
 *  Created on: 06 janvier 2026
 *      Author: Yago
 */

#include "stm32f0xx.h"
#include "main.h"
#include "bsp.h"
#include "factory_io.h"
#include "delay.h"
#include "FreeRTOSConfig.h"

//Local Static Functions
static uint8_t 	SystemClock_Config	(void);

// FreeRTOS tasks
void vTaskLed(void *pvParameters);
void vTaskControl(void *pvParameters);
void vTaskSensorMonitor(void *pvParameters);

// FreeRTOS entities
xTaskHandle			vTaskControl_handle;


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

	// Read all states from the scene
	FACTORY_IO_update();

	// Creating FreeRTOS tasks
	xTaskCreate(vTaskSensorMonitor, "Task_Sensor_Monitor", 256, NULL, 3, NULL);
	xTaskCreate(vTaskLed, "Task_LED", 128, NULL, 1, NULL);
	xTaskCreate(vTaskControl, "Task_Control", 128, NULL, 2, &vTaskControl_handle);

	// Start the Scheduler
	my_printf("Starting Scheduler...\r\n");
	vTaskStartScheduler();

	// Loop forever
	while(1)
	{
		// Should not be here
	}
}

// Task Sensor Monitor receives value of sensor and notifies tasks.
void vTaskSensorMonitor (void *pvParameters){
    static uint8_t last_sensor_state = 1;
    uint8_t current_sensor_state;

    while(1){
    	// DEVE SE LER O FACTORY_IO_update(); TODA VEZ ANTES DE PUXAR O GET, CHECAR EMAILS.
        current_sensor_state = FACTORY_IO_Sensors_Get(0x00000002);

        if (current_sensor_state == 0 && last_sensor_state == 1) {
            xTaskNotifyGive(vTaskControl_handle);
        }

        last_sensor_state = current_sensor_state;

        vTaskDelay(20);
    }
}

/*
 *	TaskLed toggles LED every 300ms
 */
void vTaskLed (void *pvParameters)
{
	my_printf("task1: vou rodar!\n\r");
	while(1)
	{
		BSP_LED_Toggle();
		vTaskDelay(100);
	}
}

/*
 *	TaskControl controls conveyor activation
 */
void vTaskControl (void *pvParameters) {
	my_printf("Waiting for button...\n\r");

	while (1) {

		if (BSP_PB_GetState() == 1) {
			// Start conveyor A[0] = 1
			my_printf("Starting Conveyor\r\n");
			FACTORY_IO_Actuators_Set(0x00000001);

			//Awaits the notification from Task Sensor Monitor
			my_printf("Waiting for sensor...\r\n");
			ulTaskNotifyTake(pdTRUE, 0);
			ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
			// Reaching this point means that a notification has been received
			// Stop conveyor A[0] = 0
			my_printf("Stop!\r\n");
			FACTORY_IO_Actuators_Set(0x00000000);
		}
		vTaskDelay(50);
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
