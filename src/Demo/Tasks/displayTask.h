#ifndef __DISPLAY_TASK_H__
#define __DISPLAY_TASK_H__

#include <FreeRTOS.h>
#include <queue.h>

#define CONSOLE_GLOBAL          1
#define CONSOLE_WIFI            2

extern xQueueHandle g_pLCDQueue;

 struct AMessage
 {
    char consoleID;
    char message[80];
 } xMessage;

void guiThread();

#endif // __DISPLAY_TASK_H__
