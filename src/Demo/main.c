#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <task.h>

#include "Drivers/bcm2835.h"
#include "Drivers/bcm2835_intc.h"
#include "Drivers/interrupts.h"
#include "Drivers/gpio.h"
#include "Drivers/uart.h"

#include "displayTask.h"

#include "gfx.h"

#include "constants.h"

#define BACKLIGHT_PWM_CHANNEL           0

static int irq_fired = 0;
/*
 *	This is the GPIO interrupt service routine, note. no SAVE/RESTORE_CONTEXT here
 *	as thats done in the bottom-half of the ISR.
 */
void vGPIOISR(int nIRQ, void *pParam )
{
	(void)nIRQ;
	(void)pParam;
	
	if(bcm2835_gpio_eds(27) == HIGH)
	{
		//SetGpio(47, 0);
		irq_fired++;
		bcm2835_gpio_set_eds(27);
	}
}


void task1() {
    //~ struct AMessage testMessage;
	//~ int i = 0;
    //~ char *entry;
	while(1) {
		//~ i++;
		SetGpio(47, 1);
        //~ testMessage.consoleID = CONSOLE_WIFI;
        //~ entry = (char *) malloc(1024*1024 * sizeof(char));
        //~ if (!entry)
        //~ {
            //~ sprintf(testMessage.message, "Malloc failed\r\n");
        //~ }
        //~ else
        //~ {
            //~ sprintf(entry, "Hello");
            //~ sprintf(testMessage.message, "Ptr address: %u\r\n", (unsigned int)entry);
        //~ }
        //~ if(entry[0] == 'H')
            //~ xQueueSend(g_pLCDQueue, &testMessage, 0);
        //~ else
        //~ {
            //~ testMessage.consoleID = CONSOLE_GLOBAL;
            //~ xQueueSend(g_pLCDQueue, &testMessage, 0);
        //~ }
        //~ free(entry);
		vTaskDelay(2000/portTICK_RATE_MS);
	}
}

void task2() {
    struct AMessage testMessage;
	//~ int i = 0;
	while(1) {
		//~ i++;
		vTaskDelay(1000/portTICK_RATE_MS);
		SetGpio(47, 0);
        testMessage.consoleID = CONSOLE_GLOBAL;
        //~ sprintf(testMessage.message, "Task 2 %d\r\n", i);
        sprintf(testMessage.message, "Interrupts fired: %d\r\n", irq_fired);
        xQueueSend(g_pLCDQueue, &testMessage, 0);
		vTaskDelay(1000/portTICK_RATE_MS);
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
    bcm2835_aux_muart_transfernb(UART_CMD_CLEAR_SCREEN);
    
    bcm2835_aux_muart_transfernb(" ################################\r");
    bcm2835_aux_muart_transfernb(" #                              #\r");
    bcm2835_aux_muart_transfernb(" # Raspberry Pi bare metal demo #\r");
    bcm2835_aux_muart_transfernb(" #                              #\r");
    bcm2835_aux_muart_transfernb(" #       - FreeRTOS v10         #\r");
    bcm2835_aux_muart_transfernb(" #       - uGFX v2.4            #\r");
    bcm2835_aux_muart_transfernb(" ################################\r\n");
    
    SetGpioFunction(47, 1);			// Act led
    SetGpioDirection(47, 1);        // Set LED as Output.
    SetGpio(47, 1);                 // Set LED off
    
    ////SetGpioFunction(18, 1);			// Backlight Enable
    ////SetGpioDirection(18, 1);        // Set backlight enable as Output
    ////SetGpio(18, 1);                 // Enable backlight
    
    bcm2835_gpio_fsel(18, BCM2835_GPIO_FSEL_ALT5);          // Set PWM0 mode
    bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);    // 1.2MHz
    bcm2835_pwm_set_mode(BACKLIGHT_PWM_CHANNEL, 1, 1);      // PWM channel enabled
    bcm2835_pwm_set_range(BACKLIGHT_PWM_CHANNEL, 1024);     // Range: 1024
    
    bcm2835_pwm_set_data(BACKLIGHT_PWM_CHANNEL, 100);       // Half brightness
    
    
    // BCM18 is the interrupt pin
    bcm2835_gpio_fsel(27, BCM2835_GPIO_FSEL_INPT);
    //bcm2835_gpio_set_pud(27, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_ren(27);
    RegisterInterrupt(BCM2835_IRQ_ID_GPIO_0, vGPIOISR, NULL);
	EnableInterrupt(BCM2835_IRQ_ID_GPIO_0);
    ////RegisterInterrupt(BCM2835_IRQ_ID_GPIO_3, vGPIOISR, NULL);
	////EnableInterrupt(BCM2835_IRQ_ID_GPIO_3);

	xTaskCreate(task1, "LED_0", 1280, NULL, 0, NULL);
	xTaskCreate(task2, "LED_1", 1280, NULL, 0, NULL);
    
    // Create the GUI task
    xTaskCreate(guiThread, "GUI_Thread", configMINIMAL_STACK_SIZE + 1024, NULL, 3, NULL);
    
    //~ gfxInit();
    
	vTaskStartScheduler();
    
    

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		//SetGpio(47, 0);
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

//*****************************************************************************
//
//! \brief  Overwrite the GCC _sbrk function which check the heap limit related
//!         to the stack pointer.
//!			In case of freertos this checking will fail.
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void * _sbrk(uint32_t delta)
{
    extern char _end;     /* Defined by the linker */
    extern char __heap_end;
    static char *heap_end;
    static char *heap_limit;
    char *prev_heap_end;

    if(heap_end == 0)
    {
        heap_end = &_end;
        heap_limit = &__heap_end;
    }

    prev_heap_end = heap_end;
    if(prev_heap_end + delta > heap_limit)
    {
        return((void *) -1L);
    }
    heap_end += delta;
    return((void *) prev_heap_end);
}
