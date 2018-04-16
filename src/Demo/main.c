#include <FreeRTOS.h>
#include <task.h>

#include "Drivers/bcm2835.h"
#include "Drivers/interrupts.h"
#include "Drivers/gpio.h"

#include "displayTask.h"


void task1() {

	int i = 0;
	while(1) {
		i++;
		SetGpio(16, 1);
		vTaskDelay(2000);
	}
}

void task2() {

	int i = 0;
	while(1) {
		i++;
		vTaskDelay(1000);
		SetGpio(16, 0);
		vTaskDelay(1000);
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
    bcm2835_gpio_fsel(14, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(15, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(16, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(17, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(20, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(21, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(22, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(23, BCM2835_GPIO_FSEL_ALT2);
    bcm2835_gpio_fsel(24, BCM2835_GPIO_FSEL_ALT2);

	//SetGpioFunction(16, 1);			// RDY led

	//xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);
	//xTaskCreate(task2, "LED_1", 128, NULL, 0, NULL);
    
    // Create the initialization task
    xTaskCreate(taskInit, "GUI Initialisation", 128, 0, 3, 0);

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
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

