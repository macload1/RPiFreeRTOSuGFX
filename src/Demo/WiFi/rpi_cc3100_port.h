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


typedef void (*P_EVENT_HANDLER)(void* pValue);

 /*!
	\brief type definition for a time value

	\note	On each porting or platform the type could be whatever is needed - integer, pointer to structure etc.
*/
//typedef unsigned long OsTime_t;

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




//extern void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent);





//~ /*!
    //~ \brief GPIO interrupt handler for the host IRQ line.

    //~ \param             index    -    GPIO index corresponding to the GPIO interrupt that was dispatched.

    //~ \sa             NwpRegisterInterruptHandler

    //~ \note            This callback fires when pin P2.5 is set high by the network processor.

    //~ \warning
 //~ */
//~ extern void HostIrqGPIO_callback(uint_least8_t index);

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
//void * mem_Malloc(unsigned long Size);


/*!
    \brief				Deallocates Memory
	\param	pMem		-	Pointer to the Buffer to be freed
	\return void
    \sa
    \note
    \warning
*/
//void mem_Free(void *pMem);

//~ #ifdef  __cplusplus
//~ }
//~ #endif // __cplusplus

//~ #endif
