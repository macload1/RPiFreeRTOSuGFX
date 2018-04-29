/******************************************************************************
*     rpi_cc3120_port.c
*
*    Simplelink Wi-Fi platform abstraction file for Raspberry Pi
******************************************************************************/
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "Drivers/bcm2835.h"
#include "Drivers/interrupts.h"
#include "Drivers/gpio.h"

#include <ti/drivers/net/wifi/porting/rpi_cc3120_port.h>
//~ #include <ti/drivers/net/wifi/simplelink.h>

/****************************************************************************
   PRIVATE VARIABLES
****************************************************************************/
static P_EVENT_HANDLER pIrqEvtHdlr;
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

//~ /****************************************************************************
   //~ GLOBAL VARIABLES
//~ ****************************************************************************/
//~ volatile Fd_t g_SpiFd = 0;
//~ SL_P_EVENT_HANDLER g_Host_irq_Hndlr = NULL;

//~ /****************************************************************************
   //~ CONFIGURATION VARIABLES
//~ ****************************************************************************/
//~ extern const WiFi_Config WiFi_config[];
//~ extern const uint_least8_t WiFi_count;
//~ static WIFIMSP432_HWAttrsV1* curDeviceConfiguration;

//~ /****************************************************************************
   //~ CONFIGURATION FUNCTION DEFINITION
//~ ****************************************************************************/
//~ void WiFi_init()
//~ {
    //~ /* We need to have at least one WiFi module. */
    //~ if(WiFi_count == 0)
    //~ {
        //~ return;
    //~ }

    //~ curDeviceConfiguration = (WIFIMSP432_HWAttrsV1*) WiFi_config[0].hwAttrs;
//~ }

//~ /****************************************************************************
   //~ LOCAL FUNCTION DEFINITIONS
//~ ****************************************************************************/
#define	ASSERT_CS()					SetGpio(26, 0);
#define	DEASSERT_CS()				SetGpio(26, 1);

Fd_t spi_Open(char *ifName,
              unsigned long flags)
{
	(void) *ifName;
	(void) flags;
	
	SetGpioFunction(26, 1);			// Set CS line as GPIO
    SetGpioDirection(26, 1);        // Set CS line as Output
	SetGpio(26, 1);					// Set CS line high
    
	bcm2835_spi_begin();
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);	// +- 4MHz
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);	// no effect
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);					// Mode 0
	
	pIrqEvtHdlr = NULL;
	
    return 1;
}

int spi_Close(Fd_t fd)
{
	(void) fd;
    bcm2835_spi_end();
    return (0);
}

int spi_Read(Fd_t fd,
             unsigned char *pBuff,
             int len)
{
    ASSERT_CS();

    /* check if the link SPI has been initialized successfully */
    if(fd < 0)
    {
        DEASSERT_CS();
        return (-1);
    }

    bcm2835_spi_transfern((char*)pBuff, len);

    DEASSERT_CS();

    return len;
}

int spi_Write(Fd_t fd,
              unsigned char *pBuff,
              int len)
{
    ASSERT_CS();

    /* check if the link SPI has been initialized successfully */
    if(fd < 0)
    {
        DEASSERT_CS();
        return (-1);
    }

    bcm2835_spi_transfern((char*)pBuff, len);

    DEASSERT_CS();

    return len;
}

int NwpRegisterInterruptHandler(P_EVENT_HANDLER InterruptHdl,
                                void* pValue)
{
	pIrqEvtHdlr = InterruptHdl;
	
	return 0;
	
    // /* Check for unregister condition */
    // if(NULL == InterruptHdl)
    // {
        // GPIO_disableInt(curDeviceConfiguration->hostIRQPin);
        // GPIO_clearInt(curDeviceConfiguration->hostIRQPin);
        // g_Host_irq_Hndlr = NULL;
        // return (0);
    // }
    // else if(NULL == g_Host_irq_Hndlr)
    // {
        // g_Host_irq_Hndlr = InterruptHdl;
        // GPIO_setCallback(curDeviceConfiguration->hostIRQPin,
                         // HostIrqGPIO_callback);
        // GPIO_clearInt(curDeviceConfiguration->hostIRQPin);
        // GPIO_enableInt(curDeviceConfiguration->hostIRQPin);
        // return (0);
    // }
    // else
    // {
        // /* An error occurred */
        // return (-1);
    // }
}

//~ void HostIrqGPIO_callback(uint_least8_t index)
//~ {
    //~ if((index == curDeviceConfiguration->hostIRQPin)
       //~ && (NULL != g_Host_irq_Hndlr))
    //~ {
        //~ g_Host_irq_Hndlr(0);
    //~ }
//~ }

//~ void NwpMaskInterrupt()
//~ {
//~ }

//~ void NwpUnMaskInterrupt()
//~ {
//~ }

void NwpPowerOnPreamble(void)
{
    SetGpioFunction(25, 1);			// Set nHIB line as GPIO
    SetGpioDirection(25, 1);        // Set nHIB line as Output
}

void NwpPowerOn(void)
{
	SetGpio(25, 1);					// Set nHIB line high
    /* wait 5msec */
    vTaskDelay(5);
}

void NwpPowerOff(void)
{
	SetGpio(25, 0);					// Set nHIB line low
    /* wait 5msec */
    vTaskDelay(5);
}

/*!
	\brief 	This function creates a sync object

	The sync object is used for synchronization between different thread or ISR and
	a thread.

	\param	pSemHandle	-	pointer to the sync object control block

	\return upon successful creation the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
int SemaphoreP_create(SemaphoreHandle_t* pSemHandle)
{
    //Check for NULL
    if(NULL == pSemHandle)
    {
        return OS_INVALID_PARAMS;
    }
    SemaphoreHandle_t *pl_SyncObj = (SemaphoreHandle_t *)pSemHandle;

    *pl_SyncObj = xSemaphoreCreateBinary();

    if((SemaphoreHandle_t)(*pSemHandle) != NULL)
    {
        return OS_OK; 
    }
    else
    {
        return OS_OPERATION_FAILED;
    }
}

/*!
	\brief 	This function deletes a sync object

	\param	pSemHandle	-	pointer to the sync object control block

	\return upon successful deletion the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
int SemaphoreP_delete(SemaphoreHandle_t* pSemHandle)
{
	//Check for NULL
	if(NULL == pSemHandle)
	{
		return OS_INVALID_PARAMS;
	}
    vSemaphoreDelete(*pSemHandle );
    return OS_OK;
}

/*!
	\brief 		This function generates a sync signal for the object.

	All suspended threads waiting on this sync object are resumed

	\param		pSemHandle	-	pointer to the sync object control block

	\return 	upon successful signaling the function should return 0
				Otherwise, a negative value indicating the error code shall be returned
	\note		the function could be called from ISR context
	\warning
*/
int SemaphoreP_post(SemaphoreHandle_t* pSemHandle)
{
	//Check for NULL
	if(NULL == pSemHandle)
	{
		return OS_INVALID_PARAMS;
	}

    if(xSemaphoreGive( *pSemHandle ) != pdTRUE)
	{
        //In case of Semaphore, you are expected to get this if multiple sem
        // give is called before sem take
        return OS_OK;
	}
	
    return OS_OK;
}

/*!
	\brief 		This function generates a sync signal for the object
				from ISR context.

	All suspended threads waiting on this sync object are resumed

	\param		pSemHandle	-	pointer to the sync object control block

	\return 	upon successful signalling the function should return 0
				Otherwise, a negative value indicating the error code shall be returned
	\note		the function is called from ISR context
	\warning
*/
int SemaphoreP_post_from_isr(SemaphoreHandle_t* pSemHandle)
{
	//Check for NULL
	if(NULL == pSemHandle)
	{
		return OS_INVALID_PARAMS;
	}
	xHigherPriorityTaskWoken = pdFALSE;
	if(pdTRUE == xSemaphoreGiveFromISR( *pSemHandle, &xHigherPriorityTaskWoken ))
	{
		if( xHigherPriorityTaskWoken )
		{
			taskYIELD ();
		}
		return OS_OK;
	}
	else
	{
		//In case of Semaphore, you are expected to get this if multiple sem
		// give is called before sem take
		return OS_OK;
	}
}


/*!
	\brief 	This function waits for a sync signal of the specific sync object

	\param	pSyncObj	-	pointer to the sync object control block
	\param	Timeout		-	numeric value specifies the maximum number of mSec to
							stay suspended while waiting for the sync signal
							Currently, the simple link driver uses only two values:
								- OS_WAIT_FOREVER
								- OS_NO_WAIT

	\return upon successful reception of the signal within the timeout window return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
int SemaphoreP_pend(SemaphoreHandle_t* pSemHandle, OsTime_t Timeout)
{
	//Check for NULL
	if(NULL == pSemHandle)
	{
		return OS_INVALID_PARAMS;
	}
    if(pdTRUE == xSemaphoreTake( (SemaphoreHandle_t)*pSemHandle, ( TickType_t )(Timeout/portTICK_PERIOD_MS) ))
    {
        return OS_OK;
    }
    else
    {
        return OS_OPERATION_FAILED;
    }
}

int Mutex_create(SemaphoreHandle_t* pMutexHandle)
{
    //Check for NULL
    if(NULL == pMutexHandle)
    {
            return OS_INVALID_PARAMS;
    }
    *pMutexHandle = xSemaphoreCreateMutex();
    if(pMutexHandle != NULL)
    {  
        return OS_OK;
    }
    else
    {
        return OS_OPERATION_FAILED;
    }
}

int Mutex_delete(SemaphoreHandle_t* pMutexHandle)
{
    vSemaphoreDelete((SemaphoreHandle_t)*pMutexHandle );
    return OS_OK;
}

int Mutex_unlock(SemaphoreHandle_t* pMutexHandle)
{
	//Check for NULL
	if(NULL == pMutexHandle)
	{
		return OS_INVALID_PARAMS;
	}
	//Release Semaphore
    if(pdTRUE == xSemaphoreGive( *pMutexHandle ))
    {
    	return OS_OK;
    }
    else
    {
    	return OS_OPERATION_FAILED;
    }
}

int Mutex_lock(SemaphoreHandle_t* pMutexHandle, OsTime_t Timeout)
{
    //Check for NULL
    if(NULL == pMutexHandle)
    {
            return OS_INVALID_PARAMS;
    }
    //Take Semaphore
    if(pdTRUE == xSemaphoreTake( *pMutexHandle, ( TickType_t ) (Timeout/portTICK_PERIOD_MS) ))
    {
        return OS_OK;
    }
    else
    {
        return OS_OPERATION_FAILED;
    }
}


/*!
	\brief 	This function to call the memory de-allocation function of the FREERTOS

	\param	Size	-	size of memory to alloc in bytes

	\return - void *
	\note
	\warning
*/

void * mem_Malloc(unsigned long Size)
{
  
    return ( void * ) pvPortMalloc( (size_t)Size );
}

/*!
	\brief 	This function to call the memory de-allocation function of the FREERTOS

	\param	pMem		-	pointer to the memory which needs to be freed
	
	\return - void 
	\note
	\warning
*/
void mem_Free(void *pMem)
{
    vPortFree( pMem );
}


//~ unsigned long TimerGetCurrentTimestamp()
//~ {
    //~ return (ClockP_getSystemTicks());
//~ }
