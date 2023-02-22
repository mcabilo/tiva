/*
 * Name: 010_basic-adc
 * Ver: 2023-Feb-22
 * Author: Mark Anthony Cabilo
 *
 * Description:
 * Demonstrate the use of the DMA peripheral in Tiva C Launchpad.
 *
 */

/**
 * HEADER FILES
 */
#include "stdint.h"                 // standard integer library
#include "stdbool.h"                // standard boolean library
#include "inc/hw_memmap.h"          // macros defining the memory map of the device
#include "inc/hw_ints.h"            // macros that define the interrupt assignment on Tiva C MCUs
#include "inc/hw_adc.h"             // macros used when accessing ADC hardware
#include "driverlib/sysctl.h"       // system control API
#include "driverlib/gpio.h"         // general purpose input output API
#include "driverlib/pin_map.h"      // pin mapping of alternative functions
#include "driverlib/adc.h"          // analog-to-digital converter API
#include "driverlib/interrupt.h"    // interrupt API
#include "driverlib/udma.h"         // micro-direct memory access API
#include "driverlib/timer.h"        // general purpose timers API
#include "driverlib/rom_map.h"      // macros for memory-saving API calls
#include "driverlib/uart.h"         // universal asynchronous receiver transmitter API
#include "utils/uartstdio.h"        // utility library for easier serial writing

/**
 * MACROS
 */
#define BUFFER_SIZE 256

/**
 * GLOBAL VARIABLES
 */
#pragma DATA_ALIGN(pui8DMAControlTable, 1024)
uint8_t pui8DMAControlTable[1024];      // application must allocate the channel control table that must be 1024-byte aligned

static uint16_t pui16ADCBuffer1[BUFFER_SIZE];
static uint16_t pui16ADCBuffer2[BUFFER_SIZE];

enum BUFFER_STATUS
{
    EMPTY,
    FILLING,
    FULL
};
static enum BUFFER_STATUS pui32BufferStatus[2];
static uint32_t g_ui32DMAErrCount = 0u;


void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    MAP_UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}


/**
 * uDMA Error Handler
 */
void
uDMAErrorHandler(void)
{
    uint32_t ui32Status;

    //
    // Check for uDMA error bit.
    //
    ui32Status = uDMAErrorStatusGet();

    //
    // If there is a uDMA error, then clear the error and increment
    // the error counter.
    //
    if(ui32Status)
    {
        uDMAErrorStatusClear();
        g_ui32DMAErrCount++;
    }
}

/**
 * ISR for ADC0 SS0
 */
void
ADCSeq0Handler(void)
{
    //
    // Clear the Interrupt Flag.
    //
    ADCIntClear(ADC0_BASE, 0);

    //
    // Determine which buffer as been filled based on the UDMA_MODE_STOP flag
    // and update the buffer status.
    //
    if ((uDMAChannelModeGet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT) ==
                               UDMA_MODE_STOP) &&
                              (pui32BufferStatus[0] == FILLING))
   {
       pui32BufferStatus[0] = FULL;
       pui32BufferStatus[1] = FILLING;
   }
   else if ((uDMAChannelModeGet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT) ==
                                UDMA_MODE_STOP) &&
                               (pui32BufferStatus[1] == FILLING))
   {
       pui32BufferStatus[0] = FILLING;
       pui32BufferStatus[1] = FULL;
   }
}



/**
 * main.c
 */
void main(void)
{
    /**
     * Application:
     * We will enable DMA controller to manage ADC data
     * and store them into memory
     *
     * Note: cannot use debugger to view data because DMA runs separately from processor (debugger is based on processor)
     *
     */

    // LOCAL VARIABLES
    uint32_t ui32AveData1, ui32AveData2, ui32Count;
    uint32_t ui32SamplesTaken = 0;
    pui32BufferStatus[0] = FILLING;
    pui32BufferStatus[1] = EMPTY;

    // A. System level configuration
    // 1. Setup system clock
    MAP_SysCtlClockSet( SYSCTL_OSC_MAIN | SYSCTL_USE_OSC );     // Use 40MHz clock

    // 2. Enable GPIO first (ADC channel 0 is located at PE3)
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );
    while( !MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOE )) {}

    // 3. Enable the ADC0 module
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while( !MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_ADC0 )) {}

    // 4. Enable the DMA peripheral
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_UDMA );
    while( !MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_UDMA )) {}

    // 5. Enable the timer peripheral
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_TIMER0 );
    while( !MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_TIMER0 )) {}

    // Optional: Configure UART for demo
    ConfigureUART();
    UARTprintf("\nTimer->ADC->uDMA demo!\n\n");
    UARTprintf("ui32AveData1\tui32AveData2\tTotal Samples\n");

    // B. Peripheral level configuration
    // 5. Configure PE3 to use its ADC function
    MAP_GPIOPinTypeADC( GPIO_PORTE_BASE, GPIO_PIN_3 );

    // 6. Configure ADC0 SS0
    ADCClockConfigSet( ADC0_BASE, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_HALF , 1 );
    SysCtlDelay(10);

    IntDisable(INT_ADC0SS0);
    IntRegister(INT_ADC0SS0, ADCSeq0Handler);
    ADCIntDisable(ADC0_BASE, 0);
    ADCSequenceDisable(ADC0_BASE, 0);
    MAP_ADCSequenceConfigure( ADC0_BASE, 0, ADC_TRIGGER_TIMER, 0 );
    MAP_ADCSequenceStepConfigure( ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0 );
    MAP_ADCSequenceEnable( ADC0_BASE, 0 );
    ADCIntClear(ADC0_BASE, 0);

    // 7. Configure uDMA controller
    MAP_uDMAEnable();
    uDMAControlBaseSet( pui8DMAControlTable );
    uDMAChannelAttributeDisable( UDMA_CHANNEL_ADC0 , UDMA_ATTR_ALTSELECT | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK );
    uDMAChannelControlSet( UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT , UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1 );
    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT, UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1);
    uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
                               UDMA_MODE_PINGPONG,
                               (void *)(ADC0_BASE + ADC_O_SSFIFO0),
                               &pui16ADCBuffer1, BUFFER_SIZE);
    uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
                               UDMA_MODE_PINGPONG,
                               (void *)(ADC0_BASE + ADC_O_SSFIFO0),
                               &pui16ADCBuffer2, BUFFER_SIZE);
    uDMAChannelAttributeEnable( UDMA_CHANNEL_ADC0 , UDMA_ATTR_USEBURST );
    uDMAChannelEnable(UDMA_CHANNEL_ADC0);

    // 8. Wrap up ADC-DMA configuration (enabling adc interrupt after dma is configured)
    ADCSequenceDMAEnable(ADC0_BASE, 0);
    ADCIntEnable(ADC0_BASE, 0);
    IntEnable(INT_ADC0SS0);

    // 9. Configure Timer for ADC sampling
    TimerConfigure(TIMER0_BASE,TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet()/16000) - 1);
    TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);



    while(1) {
        if(pui32BufferStatus[0] == FULL){
            //process data
            for(ui32Count = 0; ui32Count < BUFFER_SIZE; ui32Count++){
                ui32AveData1 += pui16ADCBuffer1[ui32Count];
                pui16ADCBuffer1[ui32Count] = 0;
            }

            pui32BufferStatus[0] = EMPTY;
            uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
                                              UDMA_MODE_PINGPONG,
                                              (void *)(ADC0_BASE + ADC_O_SSFIFO0),
                                              &pui16ADCBuffer1, BUFFER_SIZE);
            uDMAChannelEnable(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT);

            ui32AveData1 = ui32AveData1 / BUFFER_SIZE;
            ui32SamplesTaken += BUFFER_SIZE;
        }



        if(pui32BufferStatus[1] == FULL){
           //process data
           for(ui32Count = 0; ui32Count < BUFFER_SIZE; ui32Count++){
               ui32AveData2 += pui16ADCBuffer2[ui32Count];
               pui16ADCBuffer2[ui32Count] = 0;
           }

           pui32BufferStatus[1] = EMPTY;
           uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
                                             UDMA_MODE_PINGPONG,
                                             (void *)(ADC0_BASE + ADC_O_SSFIFO0),
                                             &pui16ADCBuffer2, BUFFER_SIZE);
           uDMAChannelEnable(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT);

           ui32AveData2 = ui32AveData2 / BUFFER_SIZE;
           ui32SamplesTaken += BUFFER_SIZE;

           UARTprintf("\t%4d\t\t%4d\t\t%d\r", ui32AveData1, ui32AveData2, ui32SamplesTaken);
        }

    }
}

