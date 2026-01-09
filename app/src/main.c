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
void vTaskDistribuition(void *pvParameters);

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

	/*
	 *  righ now tracealyzer doesn't show much info... so its better to leave it like this
	 *  This means TraceFacility its also deactivated in FreeRTOSConfig.h
	 *  The program doesnt run if trace is activated but you havent opened Percepio Tracealyzer (so we better leave all commented right now)
	 */
//	xTraceEnable(TRC_START);    // <====== We uncomment that when we want to see the results in tracealyzer

	// Read all states from the scene
	FACTORY_IO_update();

	/*
	 * ACTIVATES SOME ACTUATORS
	 * This activates the first 2 tapis of the factory scene.
	 * !!!! see the defines of main.h and also the comments in factory_io.h !!!!
	 */
	FACTORY_IO_Actuators_Set(4102); // right now this is a gambiarra (soma de varios binarios para ativar os actuators certos. tem que aprender a fazer do jeito certo.

	// Creating FreeRTOS tasks
	xTaskCreate(vTaskSensorMonitor, "Task_Sensor_Monitor", 256, NULL, 3, NULL);
	xTaskCreate(vTaskLed, "Task_LED", 128, NULL, 1, NULL);
	xTaskCreate(vTaskControl, "Task_Control", 128, NULL, 2, &vTaskControl_handle);
	xTaskCreate(vTaskDistribuition, "Task_Distribuition", 256, NULL, 2, NULL);

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
    static uint8_t last_sensor_state = 1;
    uint8_t current_sensor_state;
    // Resource:
    uint8_t space = 2;

    while(1){
    	// DEVE SE LER O factoryIO COM update TODA VEZ ANTES DE PUXAR O GET, CHECAR EMAILS.
//    	FACTORY_IO_update();
        current_sensor_state = FACTORY_IO_Sensors_Get(4);

        if (current_sensor_state == 0 && last_sensor_state == 1) {
        	space--;
        	if (space==0) {
        		xTaskNotifyGive(vTaskControl_handle);
        		space = 2;
        	}
        }

        last_sensor_state = current_sensor_state;

        vTaskDelay(20);
    }
}

/*
 * Click at the blue button at your board, this generates new packages (cartons) to the scene.
 */
void vTaskDistribuition (void *pvParameters) {
	my_printf("Distribuition Ready!\r\n");

	while (1) {
		if (BSP_PB_GetState() == 1) {

			my_printf("We pop a cartao!\r\n");
			FACTORY_IO_Actuators_Set(4103);
			vTaskDelay(3000); // time to make 2 packages spawn
			FACTORY_IO_Actuators_Set(4102);
		}
		vTaskDelay(50);
	}
}
/*
 *	TaskLed toggles LED every 300ms
 *	we can remove that, bernardao
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
 *	TaskControl controls "2. BLOCAGE ENTREE PALETTISEUR"
 *	It's the barrier that let the packages in to one spot before the elevator
 *
 */
void vTaskControl (void *pvParameters) {
	while (1) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		FACTORY_IO_Actuators_Set(4098);
		vTaskDelay(1500); // time to let two packages pass.
		FACTORY_IO_Actuators_Set(4102);
	}
		vTaskDelay(50);
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
