//~ /*
 //~ *   Copyright (C) 2015 Texas Instruments Incorporated
 //~ *
 //~ *   All rights reserved. Property of Texas Instruments Incorporated.
 //~ *   Restricted rights to use, duplicate or disclose this code are
 //~ *   granted through contract.
 //~ *
 //~ *   The program may not be used without the written permission of
 //~ *   Texas Instruments Incorporated or against the terms and conditions
 //~ *   stipulated in the agreement under which this program has been supplied,
 //~ *   and under no circumstances can it be used with non-TI connectivity device.
 //~ *
 //~ */
//~ //*****************************************************************************
//~ //     cc_pal.h
//~ //
//~ //    Simplelink Wi-Fi abstraction header file for for MSP432
//~ //*****************************************************************************

//~ #ifndef __CC31xx_PAL_H__
//~ #define    __CC31xx_PAL_H__

//~ #ifdef    __cplusplus
//~ extern "C" {
//~ #endif

//~ #include <time.h>

#define MAX_QUEUE_SIZE                    	(4)
#define OS_WAIT_FOREVER                   	(0xFFFFFFFF)
#define OS_NO_WAIT                        	(0)

#define OS_OK								(0)
#define OS_FAILURE             				(-1)
#define OS_OPERATION_FAILED    				(-2)
#define OS_ABORTED             				(-3)
#define OS_INVALID_PARAMS      				(-4)
#define OS_MEMORY_ALLOCATION_FAILURE    	(-5)
#define OS_TIMEOUT                      	(-6)
#define OS_EVENTS_IN_USE                	(-7)
#define OS_EVENT_OPEARTION_FAILURE      	(-8)

typedef void (*P_EVENT_HANDLER)(void* pValue);

 /*!
	\brief type definition for a time value

	\note	On each porting or platform the type could be whatever is needed - integer, pointer to structure etc.
*/
typedef unsigned long OsTime_t;

//~ typedef struct WiFi_Config
//~ {
    //~ /*! Pointer to a driver specific hardware attributes structure */
    //~ void const *hwAttrs;
//~ } WiFi_Config;

/*!
    \brief  type definition for the SPI channel file descriptor

    \note    On each porting or platform the type could be whatever is needed - integer, pointer to structure etc.
 */
typedef int Fd_t;

//~ /*!
    //~ \brief     type definition for the host interrupt handler

    //~ \param     pValue    -    pointer to any memory strcuture. The value of this pointer is given on
                        //~ registration of a new interrupt handler

    //~ \note
 //~ */
//~ typedef void (*SL_P_EVENT_HANDLER)(unsigned int index);

//~ #define P_EVENT_HANDLER SL_P_EVENT_HANDLER

//~ /*!
    //~ \brief     type definition for the host spawn function

    //~ \param     pValue    -    pointer to any memory strcuture. The value of this pointer is given on
                        //~ invoking the spawn function.

    //~ \note
 //~ */
//~ typedef signed short (*P_OS_SPAWN_ENTRY)(void* pValue);

//~ typedef struct
//~ {
    //~ P_OS_SPAWN_ENTRY pEntry;
    //~ void* pValue;
//~ }tSimpleLinkSpawnMsg;

/*!
    \brief open spi communication port to be used for communicating with a SimpleLink device

    Given an interface name and option flags, this function opens the spi communication port
    and creates a file descriptor. This file descriptor can be used afterwards to read and
    write data from and to this specific spi channel.
    The SPI speed, clock polarity, clock phase, chip select and all other attributes are all
    set to hardcoded values in this function.

    \param             ifName        -    points to the interface name/path. The interface name is an
                                    optional attributes that the simple link driver receives
                                    on opening the device. in systems that the spi channel is
                                    not implemented as part of the os device drivers, this
                                    parameter could be NULL.
    \param            flags        -    option flags

    \return            upon successful completion, the function shall open the spi channel and return
                    a non-negative integer representing the file descriptor.
                    Otherwise, -1 shall be returned

    \sa             spi_Close , spi_Read , spi_Write
    \note
    \warning
 */
extern Fd_t spi_Open(char *ifName,
                     unsigned long flags);

/*!
    \brief closes an opened SPI communication port

    \param             fd            -    file descriptor of an opened SPI channel

    \return            upon successful completion, the function shall return 0.
                    Otherwise, -1 shall be returned

    \sa             spi_Open
    \note
    \warning
 */
extern int spi_Close(Fd_t fd);

/*!
    \brief attempts to read up to len bytes from SPI channel into a buffer starting at pBuff.

    \param             fd            -    file descriptor of an opened SPI channel

    \param            pBuff        -     points to first location to start writing the data

    \param            len            -    number of bytes to read from the SPI channel

    \return            upon successful completion, the function shall return 0.
                    Otherwise, -1 shall be returned

    \sa             spi_Open , spi_Write
    \note
    \warning
 */
extern int spi_Read(Fd_t fd,
                    unsigned char *pBuff,
                    int len);

/*!
    \brief attempts to write up to len bytes to the SPI channel

    \param             fd            -    file descriptor of an opened SPI channel

    \param            pBuff        -     points to first location to start getting the data from

    \param            len            -    number of bytes to write to the SPI channel

    \return            upon successful completion, the function shall return 0.
                    Otherwise, -1 shall be returned

    \sa             spi_Open , spi_Read
    \note            This function could be implemented as zero copy and return only upon successful completion
                    of writing the whole buffer, but in cases that memory allocation is not too tight, the
                    function could copy the data to internal buffer, return back and complete the write in
                    parallel to other activities as long as the other SPI activities would be blocked untill
                    the entire buffer write would be completed
    \warning
 */
extern int spi_Write(Fd_t fd,
                     unsigned char *pBuff,
                     int len);

/*!
    \brief register an interrupt handler for the host IRQ

    \param             InterruptHdl    -    pointer to interrupt handler function

    \param             pValue            -    pointer to a memory strcuture that is passed to the interrupt handler.

    \return            upon successful registration, the function shall return 0.
                    Otherwise, -1 shall be returned

    \sa
    \note            If there is already registered interrupt handler, the function should overwrite the old handler
                    with the new one
    \warning
 */
extern int NwpRegisterInterruptHandler(P_EVENT_HANDLER InterruptHdl,
                                       void* pValue);

//~ /*!
    //~ \brief                 Masks host IRQ


    //~ \sa                     NwpUnMaskInterrupt

    //~ \warning
 //~ */
//~ extern void NwpMaskInterrupt();

//~ /*!
    //~ \brief                 Unmasks host IRQ


    //~ \sa                     NwpMaskInterrupt

    //~ \warning
 //~ */
//~ extern void NwpUnMaskInterrupt();

/*!
    \brief Preamble to the enabling the Network Processor.
           Placeholder to implement any pre-process operations
           before enabling networking operations.

    \sa            sl_DeviceEnable

    \note       belongs to \ref ported_sec

 */
extern void NwpPowerOnPreamble(void);

/*!
    \brief        Enable the Network Processor

    \sa            sl_DeviceDisable

    \note       belongs to \ref ported_sec
    \note        This function asserts nHIB line (P4.1) to turn on the network processor.

 */
extern void NwpPowerOn(void);

/*!
    \brief        Disable the Network Processor

    \sa            sl_DeviceEnable

    \note       belongs to \ref ported_sec
    \note        This function de-assert nHIB line (P4.1) to turn the network processor off.

 */
extern void NwpPowerOff(void);

//~ /*!
    //~ \brief GPIO interrupt handler for the host IRQ line.

    //~ \param             index    -    GPIO index corresponding to the GPIO interrupt that was dispatched.

    //~ \sa             NwpRegisterInterruptHandler

    //~ \note            This callback fires when pin P2.5 is set high by the network processor.

    //~ \warning
 //~ */
//~ extern void HostIrqGPIO_callback(uint_least8_t index);

/*!
    \brief Creates a semaphore handle, using the driver porting layer of the core SDK.

    \param             pSemHandle      -    pointer to a memory strcuture that would contain the handle.

    \return            upon successful creation, the function shall return 0.
                    Otherwise, -1 shall be returned

    \note           belongs to \ref ported_sec
 */
extern int SemaphoreP_create(SemaphoreHandle_t* pSemHandle);

/*!
	\brief 	This function deletes a sync object

	\param	pSemHandle	-	pointer to the sync object control block

	\return upon successful deletion the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
extern int SemaphoreP_delete(SemaphoreHandle_t* pSemHandle);

/*!
	\brief 		This function generates a sync signal for the object.

	All suspended threads waiting on this sync object are resumed

	\param		pSemHandle	-	pointer to the sync object control block

	\return 	upon successful signaling the function should return 0
				Otherwise, a negative value indicating the error code shall be returned
	\note		the function could be called from ISR context
	\warning
*/
extern int SemaphoreP_post(SemaphoreHandle_t* pSemHandle);

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
extern int SemaphoreP_post_from_isr(SemaphoreHandle_t* pSemHandle);

/*!
	\brief 	This function waits for a sync signal of the specific sync object

	\param	pSyncObj	-	pointer to the sync object control block
	\param	Timeout		-	numeric value specifies the maximum number of mSec to
							stay suspended while waiting for the sync signal
							Currently, the simple link driver uses only two values:
								- OSI_WAIT_FOREVER
								- OSI_NO_WAIT

	\return upon successful reception of the signal within the timeout window return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
extern int SemaphoreP_pend(SemaphoreHandle_t* pSemHandle , OsTime_t Timeout);

/*!
    \brief Creates a mutex object handle, using the driver porting layer of the core SDK.

    \param             pMutexHandle    -    pointer to a memory strcuture that would contain the handle.

    \return            upon successful creation, the function shall return 0.
                    ~ Otherwise, -1 shall be returned
    \note           belongs to \ref ported_sec
 */
extern int Mutex_create(SemaphoreHandle_t* pMutexHandle);

/*!
	\brief 	This function deletes a locking object.

	\param	pMutexHandle	-	pointer to the locking object control block

	\return upon successful deletion the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
extern int Mutex_delete(SemaphoreHandle_t* pMutexHandle);

/*!
    \brief Unlocks a mutex object.

    \param             pMutexHandle    -    pointer to a memory strcuture that contains the object.

    \return            upon successful unlocking, the function shall return 0.

    \note           belongs to \ref ported_sec
 */
extern int Mutex_unlock(SemaphoreHandle_t* pMutexHandle);

/*!
    \brief Locks a mutex object.

    \param             pMutexHandle    -    pointer to a memory structure that contains the object.

    \return            upon successful locking, the function shall return 0.

    \note           belongs to \ref ported_sec

    \warning        The lock will block until the mutex is available.
 */
extern int Mutex_lock(SemaphoreHandle_t* pMutexHandle, OsTime_t Timeout);

//~ /*!
    //~ \brief Starts GPT A2, in order to measure timeouts and monitor time.

    //~ \sa                 TimerGetCurrentTimestamp

    //~ \note           A2 is configured as free running timer, and starts counting when 'sl_Start()'
                    //~ is invoked.

    //~ \warning        This means that timer A2 is unavailable for the user.
 //~ */
//~ extern void simplelink_timerA2_start();

//~ /*!
    //~ \brief Locks a mutex object.

    //~ \return            32-bit value of the counting in timer A2.

    //~ \sa             simplelink_timerA2_start

    //~ \warning        This means that timer A2 is unavailable for the user.
 //~ */
//~ extern unsigned long TimerGetCurrentTimestamp();

//~ /*!
    //~ \brief Initializes individual driver with given HW attributes. If not
            //~ provided the default configurations of each HW implementation will
            //~ be used.

    //~ \return         None
 //~ */
//~ extern void WiFi_init();



/*!
    \brief			  		Allocates Memory on Heap
	\param	Size		- 	Size of the Buffer to be allocated
    \sa
    \note
    \warning
*/
void * mem_Malloc(unsigned long Size);


/*!
    \brief				Deallocates Memory
	\param	pMem		-	Pointer to the Buffer to be freed
	\return void
    \sa
    \note
    \warning
*/
void mem_Free(void *pMem);

//~ #ifdef  __cplusplus
//~ }
//~ #endif // __cplusplus

//~ #endif
