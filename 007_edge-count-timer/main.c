/*
 * Name: 007_edge-count-timer
 * Ver: 2023-Jan-24
 * Author: Mark Anthony Cabilo
 *
 * Description:
 * Demonstrate the use of an edge-count timer in Tiva C Launchpad.
 * IMPORTANT!! Use a mercury contact sensor or similar two-state sensor.
 *
 */


/**
 * HEADER FILES
 */
#include "stdint.h"                 // standard integer library
#include "stdbool.h"                // standard boolean library
#include "inc/hw_ints.h"            // macros for interrupts types
#include "inc/hw_memmap.h"          // macros for memory map
#include "driverlib/sysctl.h"       // system control API
#include "driverlib/gpio.h"         // general-purpose IO API
#include "driverlib/pin_map.h"      // macros for alternate pin functions
#include "driverlib/timer.h"        // timer API
#include "driverlib/interrupt.h"    // interrupt API
#include "driverlib/rom_map.h"      // macros for memory-saving API calls
#include "driverlib/uart.h"         // UART API
#include "driverlib/rom_map.h"      // memory-saving API calls
#include "utils/uartstdio.h"        // utility library for serial printing

/**
 * GLOBAL VARIABLES
 */
uint32_t ui32InterruptCount = 0;

/**
 * ISR
 */
void counted_ten(void){
    MAP_TimerIntClear( TIMER0_BASE , TIMER_CAPA_MATCH );

    ui32InterruptCount+=10;

    // The timer stops if the capture match has triggered; so we are restarting it
    MAP_TimerEnable( TIMER0_BASE , TIMER_A );
}

/**
 * FUNCTION PROTOTYPES
 */
void SerialEnable(void);

/**
 * MAIN FUNCTION
 */
void main(void)
{
    /**
     * Application:
     * Edge-counting is only available in 16-bit mode timers. In this project,
     * we count how many times the mercury liquid has hit the base of the sensor.
     *
     * - PB6 - input pin for mercury sensor
     */

    // A. System level configuration
    // 1. Setup system clock
    MAP_SysCtlClockSet( SYSCTL_OSC_MAIN | SYSCTL_USE_OSC );     // Use 40MHz clock

    /**
     *  Serial will be used to display output; since you are not concerned with its configuration for now,
     *  I will not complicate the main function with its setup. (using UART0)
     */
    SerialEnable();

    // 2. Enable GPIOB for sensor input
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOB )){}

    // 2. Enable timer0 peripheral
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_TIMER0 );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_TIMER0 )) {}

    // B. Peripheral level configuration
    // 3. Configure PB6 as input for Timer0
    MAP_GPIOPinTypeGPIOInput( GPIO_PORTB_BASE , GPIO_PIN_6 );

    // 4. Configure PB6 alternate function
    MAP_GPIOPinTypeTimer( GPIO_PORTB_BASE , GPIO_PIN_6 );
    MAP_GPIOPinConfigure( GPIO_PB6_T0CCP0 );

    // 5. Setup PF0 to use internal pull-up resistor
    MAP_GPIOPadConfigSet( GPIO_PORTB_BASE , GPIO_PIN_6 , GPIO_STRENGTH_2MA , GPIO_PIN_TYPE_STD_WPU );

    // 4. Configure Timer0
    // Timer0A will perform edge count capture (timer0B unused)
    MAP_TimerConfigure( TIMER0_BASE , TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP );

    // 5. Configure to capture rising edges
    MAP_TimerControlEvent( TIMER0_BASE , TIMER_A , TIMER_EVENT_POS_EDGE );

    // 6. Set capture period
    MAP_TimerLoadSet( TIMER0_BASE , TIMER_A , 10 ); // will count 10 times at most before calling an interrupt

    // 7. Set timer matching condition
    MAP_TimerMatchSet( TIMER0_BASE , TIMER_A , 10 );

    // 6. Register the port-level interrupt handler
    TimerIntRegister( TIMER0_BASE , TIMER_A , counted_ten );

    // 7. Enable capture match interrupt (not the timeout interrupt!)
    MAP_TimerIntEnable( TIMER0_BASE , TIMER_CAPA_MATCH );

    // C. System level interrupt
    // 9. Set timer interrupt priority
    MAP_IntPrioritySet( INT_TIMER0A , 0 );

    // 10. Enable interrupt from peripheral
    MAP_IntEnable( INT_TIMER0A );

    // 11. Enable interrupts to the processor
    MAP_IntMasterEnable();

    uint32_t ui32EdgeCount = 0;
    uint32_t ui32LastCount = ui32EdgeCount;
    MAP_TimerEnable( TIMER0_BASE , TIMER_A );

    UARTprintf("\n\n\n\n\n\n\n\n\n\n\n\n\rEdge Counter Sample\r\n");

    while(1) {
        ui32EdgeCount = MAP_TimerValueGet(TIMER0_BASE, TIMER_A);

        if ( ui32LastCount != ui32EdgeCount ){
            ui32LastCount = ui32EdgeCount;
            UARTprintf("\rHg contacts detected: %2d\r\n", ui32LastCount);
            UARTprintf("\rIt's more than %d\r\b", ui32InterruptCount);
        }

    }
}


/**
 * FUNCTION DEFINITIONS
 */

//Serial config: 115200 baud, 8-N-1
void SerialEnable(void){
        // Enable the GPIO Peripheral used by the UART.
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

        // Enable UART0
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

        // Configure GPIO Pins for UART mode.
        MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
        MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
        MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

        // Use the internal 16MHz oscillator as the UART clock source.
        MAP_UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

        // Initialize the UART for console I/O.
        UARTStdioConfig(0, 115200, 16000000);
}
