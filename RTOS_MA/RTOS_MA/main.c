/******************************************************
FreeRTOS demo program.
Implementing 3 tasks, each blinking a LED.

Target = "Arduino Mega2560" + "PR I/O Shield"

Michael Alrøe
*******************************************************/
#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

void vLEDFlashTask1( void *pvParameters )
{
	const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
	while(1)
	{
		vTaskDelay( xDelay );
		toggleLED(0);	
	}
}

void vLEDFlashTask2( void *pvParameters )
{
	const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
	while(1)
	{
		toggleLED(1);
		vTaskDelay( xDelay );
	}
}

void vLEDFlashTask3( void *pvParameters )
{
	const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
	while(1)
	{
		toggleLED(2);
		vTaskDelay( xDelay );
	}
}

void vApplicationIdleHook( void )
{
	/* Doesn't do anything yet. */
}

int main(void)
{
	initLEDport();
	xTaskCreate( vLEDFlashTask1, "LED1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vLEDFlashTask2, "LED2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vLEDFlashTask3, "LED3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	vTaskStartScheduler();
	while(1)  // Will never go here!
	{}
}

