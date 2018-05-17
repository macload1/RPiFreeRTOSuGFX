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

#include "simplelink.h"

#include "user.h"

#include "rpi_cc3100_port.h"

//~ #include <ti/drivers/net/wifi/simplelink.h>

/****************************************************************************
   PRIVATE VARIABLES
****************************************************************************/
static P_EVENT_HANDLER pIrqEvtHdlr;
extern portBASE_TYPE xHigherPriorityTaskWoken;

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
	(void)pValue;
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
	\brief 	This function to call the memory de-allocation function of the FREERTOS

	\param	Size	-	size of memory to alloc in bytes

	\return - void *
	\note
	\warning
*/

//~ void * mem_Malloc(unsigned long Size)
//~ {
  
    //~ return ( void * ) pvPortMalloc( (size_t)Size );
//~ }

/*!
	\brief 	This function to call the memory de-allocation function of the FREERTOS

	\param	pMem		-	pointer to the memory which needs to be freed
	
	\return - void 
	\note
	\warning
*/
//~ void mem_Free(void *pMem)
//~ {
    //~ vPortFree( pMem );
//~ }


//~ unsigned long TimerGetCurrentTimestamp()
//~ {
    //~ return (ClockP_getSystemTicks());
//~ }


/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
	(void) pDevEvent;
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
    bcm2835_aux_muart_transfernb(" [GENERAL EVENT] \n\r");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
	(void) pSock;
    /*
     * This application doesn't work w/ socket - Hence not handling these events
     */
    bcm2835_aux_muart_transfernb(" [SOCK EVENT] Unexpected event \n\r");
}

/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(pWlanEvent == NULL)
    {
        bcm2835_aux_muart_transfernb(" [WLAN EVENT] NULL Pointer Error \n\r");
        return;
    }
    
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            //ET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'slWlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * slWlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
            bcm2835_aux_muart_transfernb(" [WLAN EVENT] Connect event \n\r");
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            //CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
            //CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_ACQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            /* If the user has initiated 'Disconnect' request, 'reason_code' is
             * SL_USER_INITIATED_DISCONNECTION */
            if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                /* Device disconnected from the AP on application's request */
            }
            else
            {
                bcm2835_aux_muart_transfernb(" Device disconnected from the AP on an ERROR..!! \n\r");
            }
        }
        break;

        case SL_WLAN_STA_CONNECTED_EVENT:
            /*
             * Information about the connected STA (like name, MAC etc) will be
             * available in 'slPeerInfoAsyncResponse_t' - Applications
             * can use it if required
             *
             * slPeerInfoAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.APModeStaConnected;
             *
             */
        	bcm2835_aux_muart_transfernb(" [WLAN EVENT] STA Conneected event \n\r");
            break;

        case SL_WLAN_STA_DISCONNECTED_EVENT:
            /*
             * Information about the connected STA (device name, MAC) will be
             * available in 'slPeerInfoAsyncResponse_t' - Applications
             * can use it if required
             *
             * slPeerInfoAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.APModeStaConnected;
             *
             */
        	 bcm2835_aux_muart_transfernb(" [WLAN EVENT] STA Dis Conneected event \n\r");
            break;

        default:
        {
            bcm2835_aux_muart_transfernb(" [WLAN EVENT] Unexpected event \n\r");
        }
        break;
    }
}
