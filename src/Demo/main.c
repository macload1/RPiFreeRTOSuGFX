#include <stdlib.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "Drivers/bcm2835.h"
#include "Drivers/interrupts.h"
#include "Drivers/gpio.h"
#include "Drivers/uart.h"

#include "displayTask.h"

#include "gfx.h"

void task1() {
    struct AMessage testMessage;
	int i = 0;
	while(1) {
		i++;
		SetGpio(47, 1);
        testMessage.consoleID = CONSOLE_WIFI;
        sprintf(testMessage.message, "Hello %d\r\n", i);
        xQueueSend(g_pLCDQueue, &testMessage, 0);
		vTaskDelay(200);
	}
}

void task2() {
    struct AMessage testMessage;
	int i = 0;
	while(1) {
		i++;
		vTaskDelay(100);
		SetGpio(47, 0);
        testMessage.consoleID = CONSOLE_GLOBAL;
        sprintf(testMessage.message, "Task 2 %d\r\n", i);
        xQueueSend(g_pLCDQueue, &testMessage, 0);
		vTaskDelay(100);
	}
}


/**
 *	This is the systems main entry, some call it a boot thread.
 *
 *	-- Absolutely nothing wrong with this being called main(), just it doesn't have
 *	-- the same prototype as you'd see in a linux program.
 **/
int main(void) {

	DisableInterrupts();
	InitInterruptController();

    bcm2835_init();
    bcm2835_gpio_fsel(0, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(1, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(4, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(5, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(6, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(7, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(8, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(12, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(13, BCM2835_GPIO_FSEL_ALT2);
    //bcm2835_gpio_fsel(14, BCM2835_GPIO_FSEL_ALT2);
    //bcm2835_gpio_fsel(15, BCM2835_GPIO_FSEL_ALT2);
    //bcm2835_gpio_fsel(14, BCM2835_GPIO_FSEL_ALT0);  // UART TX
    //bcm2835_gpio_fsel(15, BCM2835_GPIO_FSEL_ALT0);  // UART RX
    bcm2835_gpio_fsel(16, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(17, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(20, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(21, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(22, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(23, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(24, BCM2835_GPIO_FSEL_ALT2);
    
    // Initialise UART
    bcm2835_aux_muart_init();
    bcm2835_aux_muart_transfernb("Hello World\r\n");
    
    SetGpioFunction(47, 1);			// Act led
    SetGpioDirection(47, 1);        // Set LED as Output


	xTaskCreate(task1, "LED_0", 1280, NULL, 0, NULL);
	xTaskCreate(task2, "LED_1", 1280, NULL, 0, NULL);
    
    // Create the GUI task
    xTaskCreate(guiThread, "GUI_Thread", configMINIMAL_STACK_SIZE + 1024, NULL, 3, NULL);
    
    gfxInit();
    
	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		SetGpio(47, 0);
	}
	
	return 0;
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time task stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook	function is 
	called if a task stack overflow is detected.  Note the system/interrupt
	stack is not checked. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}

