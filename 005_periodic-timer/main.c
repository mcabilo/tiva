/*
 * Name: 005_periodic-timer
 * Ver: 2023-Jan-24
 * Author: Mark Anthony Cabilo
 *
 * Description:
 * Demonstrate the use of a periodic timer in Tiva C Launchpad
 *
 */


/**
 * HEADER FILES
 */
#include "stdint.h"                 // standard integer library
#include "stdbool.h"                // standard boolean library
#include "inc/hw_memmap.h"          // macros for memory map
#include "inc/tm4c123gh6pm.h"       // board-specific macros
#include "driverlib/sysctl.h"       // system control API
#include "driverlib/gpio.h"         // general-purpose IO API
#include "driverlib/timer.h"        // timer API
#include "driverlib/interrupt.h"    // interrupt API
#include "driverlib/rom_map.h"      // macros for memory-saving API calls

/**
 * GLOBAL VARIABLE
 */
uint8_t control = 0x0;

/**
 * ISR
 */
void toggle_color() {
    MAP_TimerIntClear( TIMER0_BASE , TIMER_BOTH );
    if (control == 0x7)
        control = 0x0;
    else
        control += 1;
}

/**
 * MAIN FUNCTION
 */
void main(void)
{
    uint32_t period;
    uint32_t state = 0x0;

    /**
     * Application:
     * LED color changes at every end of timer period
     */

    // A. System level configuration
    // 1. Setup system clock
    MAP_SysCtlClockSet( SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_5 ); // Use MOSC to drive 400MHz PLL. The use sysdiv5 to apply a /10 divisor and finally generating a 40MHz clock signal.

    // Enable peripheral for on-board LED (PF3, PF2, PF1) and push button SW1 (PF4)
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF )){}

    // 2. Enable timer0 peripheral
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_TIMER0 );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_TIMER0 )) {}

    // B. Peripheral level configuration
    // 3. Configure LED pins and SW1 pin
    MAP_GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );

    // 4. Configure Timer0
    // Timer counts down and loops.
    MAP_TimerConfigure( TIMER0_BASE , TIMER_CFG_PERIODIC );

    // Timer counts up and loops.
    //MAP_TimerConfigure( TIMER0_BASE , TIMER_CFG_PERIODIC_UP );

    // 5. Setup timer period
    period = MAP_SysCtlClockGet() / 2;
    MAP_TimerLoadSet( TIMER0_BASE, TIMER_BOTH , period - 1); // Set to 1/2 seconds

    // 6. Register the peripheral-level interrupt handler
    TimerIntRegister( TIMER0_BASE , TIMER_BOTH , toggle_color );

    // 7. Enable timer interrupt
    MAP_TimerIntEnable( TIMER0_BASE , TIMER_TIMA_TIMEOUT ); // use timerA for 32-bit timer timeout

    // C. System level interrupt
    // 8. Set timer interrupt priority
    MAP_IntPrioritySet( INT_TIMER0A , 0 );

    // 9. Enable interrupt from peripheral
    MAP_IntEnable( INT_TIMER0A );

    // 10. Enable interrupts to the processor
    MAP_IntMasterEnable();

    // 11. Enable timer
    MAP_TimerEnable( TIMER0_BASE , TIMER_BOTH );

    while(1) {
        if ( control != state ) { // only update portF if a new set of LEDs are chosen to be toggled
            MAP_GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 , 0x00 );  // turn off all LEDs
            state = control;                                                                    // set which LEDs are to turn on this time
            MAP_GPIOPinWrite( GPIO_PORTF_BASE , state << 1 , 0x0E );                            // move by 1 to the left (since PF0 is a switch!) to switch on corresponding LEDs
        }
    }
}
