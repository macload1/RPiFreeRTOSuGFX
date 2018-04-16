#include <FreeRTOS.h>

#include <task.h>

#include "displayTask.h"

#include "gfx.h"


//*****************************************************************************
//
// The item size and queue size for the LCD message queue.
//
//*****************************************************************************
#define LCD_ITEM_SIZE           sizeof(char*)
#define LCD_QUEUE_SIZE          20


//*****************************************************************************
//
// The queue that holds messages sent to the LCD task.
//
//*****************************************************************************
xQueueHandle g_pLCDQueue;


/* The handles for three consoles */
GHandle GW1;


void uGFXMain(void)
{
    coord_t	width, height;
	// Get the screen size
    width = gdispGetWidth();
    height = gdispGetHeight();

    //~ font_t	font1;
	//~ /* Set some fonts */
	//~ font1 = gdispOpenFont("UI2");
	//~ //font1 = gdispOpenFont("DejaVu Sans 12");
	//~ gwinSetDefaultFont(font1);
    
    //~ //
    //~ // Create a queue for sending messages to the LED task.
    //~ //
    //~ //g_pLCDQueue = xQueueCreate(LCD_QUEUE_SIZE, LCD_ITEM_SIZE);
    //~ /* We want this queue to be viewable in a RTOS kernel aware debugger, so register it. */
	//~ //vQueueAddToRegistry( g_pLCDQueue, "LCDQueue" );
    


	//~ /* create the console window */
    //~ GWindowInit	wi;

    //~ wi.show = TRUE;
    //~ wi.x = 5;
    //~ wi.y = 5;
    //~ wi.width = width - 10;
    //~ wi.height = height - 10;
    //~ GW1 = gwinConsoleCreate(0, &wi);
    
	//~ /* Use a special font for GW1 */
	//~ gwinSetFont(GW1, font1);
	//~ /* Set the fore- and background colors for each console */
	//~ gwinSetColor(GW1, Green);
	//~ gwinSetBgColor(GW1, Red);
	//~ /* clear all console windows - to set background */
	//~ gwinClear(GW1);
	//~ /* Output some data on the first console */
    //~ gwinPrintf(GW1, "Hello World\r\n");
	//~ gwinPrintf(GW1, "Welcome \033bhuman\033B!\r\n");

	//~ //char *Message;
    //~ //
    //~ // Loop forever.
    //~ //
    //~ while(1)
    //~ {
    	//
		// Read the next message, if available on queue.
		//
		//~ if(xQueueReceive(g_pLCDQueue, &Message, portMAX_DELAY) == pdPASS)
		//~ {
			//~ gwinPrintf(GW1, Message);
		//~ }
        //~ vTaskDelay(1000);
        //~ gwinPrintf(GW1, "Welcome \033bTobias\033B!\r\n");
    //~ }
    
    


    //~ // Code Here
	//~ gdispDrawBox(10, 10, width/2, height/2, Yellow);
    //~ gdispFillArea(width/2, height/2, width/2-10, height/2-10, Red);
    //~ gdispDrawLine(5, 30, width-50, height-40, Blue);

	while(1)
	{
		vTaskDelay(1000);
        gdispDrawBox(10, 10, width/2, height/2, Yellow);
		vTaskDelay(1000);
        gdispFillArea(width/2, height/2, width/2-10, height/2-10, Red);
		vTaskDelay(1000);
        gdispDrawLine(5, 30, width-50, height-40, Blue);
        //~ vTaskDelay(1000);
        //~ gdispDrawString(20, 20, "Hello World", font1, Yellow);
	}

    return;
}

void taskInit(void* pvParameters)
{
    (void)pvParameters;
 
    gfxInit();
}

