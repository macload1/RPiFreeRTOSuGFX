#include <stdlib.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <queue.h>

#include <task.h>

#include "Drivers/bcm2835.h"


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
GHandle GW_global;
GHandle GW_wifi;




void guiThread()
{
    coord_t	width, height;
	// Get the screen size
    width = 480; //gdispGetWidth();
    height = 272; //gdispGetHeight();
    //~ width = 800; //gdispGetWidth();
    //~ height = 480; //gdispGetHeight();
    
    bcm2835_aux_muart_transfernb("Entered Display Task\r");
    font_t	font1;
	/* Set some fonts */
	//font1 = gdispOpenFont("UI2");
	font1 = gdispOpenFont("DejaVu Sans 12");
	gwinSetDefaultFont(font1);
    
	/* create the console window */
    GWindowInit	wi;

    // Global console
    wi.show = TRUE;
    wi.x = 5;
    wi.y = 5;
    wi.width = width/2 - 10;
    wi.height = height - 10;
    GW_global = gwinConsoleCreate(0, &wi);
    gdispDrawBox(2, 2, width/2 - 4, height - 4, White);
    
    // WiFi console
    wi.x = width/2 + 5;
    wi.y = 5;
    wi.width = width/2 - 10;
    wi.height = height - 10;
    GW_wifi = gwinConsoleCreate(0, &wi);
    gdispDrawBox(width/2 + 2, 2, width/2 - 4, height - 4, White);
    
	/* Use a special font for GW_global */
	gwinSetFont(GW_global, font1);
	/* Set the fore- and background colors for each console */
	gwinSetColor(GW_global, White);
	gwinSetBgColor(GW_global, Black);
	/* clear all console windows - to set background */
	gwinClear(GW_global);
	/* Output some data on the first console */
    gwinPrintf(GW_global, "Hello World\r\n");
	gwinPrintf(GW_global, "Welcome \033bhuman\033B! This is a very long sentence!\r\n");


    //
    // Create a queue for sending messages to the LED task.
    //
    g_pLCDQueue = xQueueCreate(LCD_QUEUE_SIZE, sizeof( struct AMessage ) );
    /* We want this queue to be viewable in a RTOS kernel aware debugger, so register it. */
	vQueueAddToRegistry( g_pLCDQueue, "LCDQueue" );
    if( g_pLCDQueue == 0 )
    {
        // Failed to create the queue.
        gwinPrintf(GW_global, "Failed to create the queue.\r\n");
    }
    
	//int mode = ROLE_STA;
    //mode = sl_Start(0, 0, 0);
    //sl_Start(0, 0, 0);

	struct AMessage pxRxedMessage;
    //
    // Loop forever.
    //
    while(1)
    {
    	//
		// Read the next message, if available on queue.
		//
		if(xQueueReceive(g_pLCDQueue, &pxRxedMessage, portMAX_DELAY) == pdPASS)
		{
            //gwinPrintf(GW_global, "Received message\r\n");
            if(pxRxedMessage.consoleID == CONSOLE_GLOBAL)
                gwinPrintf(GW_global, pxRxedMessage.message);
            else if(pxRxedMessage.consoleID == CONSOLE_WIFI)
            {
                gwinPrintf(GW_wifi, pxRxedMessage.message);
            }
		}
        //bcm2835_aux_muart_transfernb("looping Display Task\r\n");
    }
    
    


    //~ // Code Here
	//~ gdispDrawBox(10, 10, width/2, height/2, Yellow);
    //~ gdispFillArea(width/2, height/2, width/2-10, height/2-10, Red);
    //~ gdispDrawLine(5, 30, width-50, height-40, Blue);
    //~ gdispDrawBox(10, 10, width/2, height/2, Yellow);

	//~ while(1)
	//~ {
		//~ vTaskDelay(1000);
        //~ gdispDrawBox(10, 10, width/2, height/2, Yellow);
		//~ vTaskDelay(1000);
        //~ gdispFillArea(width/2, height/2, width/2-10, height/2-10, Red);
		//~ vTaskDelay(1000);
        //~ gdispDrawLine(5, 30, width-50, height-40, Blue);
        //~ bcm2835_aux_muart_transfernb("looping Display Task\r\n");
        //~ vTaskDelay(1000);
        //~ gdispDrawString(20, 20, "Hello World", font1, Yellow);
	//~ }

    return;
}

