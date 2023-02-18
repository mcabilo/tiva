/*
 * Name: 004_one-shot-timer
 * Ver: 2023-Jan-24
 * Author: Mark Anthony Cabilo
 *
 * Description:
 * Demonstrate the use of on-shot timer in Tiva C Launchpad
 *
 */


/**
 * HEADER FILES
 */
#include "stdint.h"                 // standard integer library
#include "stdbool.h"                // standard boolean library
#include "inc/hw_memmap.h"          // macros for memory map
#include "driverlib/sysctl.h"       // system control API
#include "driverlib/gpio.h"         // general-purpose IO API
#include "driverlib/timer.h"        // timer API
#include "driverlib/rom_map.h"      // macros for memory-saving API calls

/**
 * MAIN FUNCTION
 */
void main(void)
{
    uint32_t period;

    /**
     * Application:
     * A green LED will blink once peripheral configuration is done.
     * The timer will start running when SW1 is pressed and a white
     * LED will light up until timer expires.
     */
    // A. System level configuration
    // 1. Setup system clock
    MAP_SysCtlClockSet( SYSCTL_OSC_MAIN | SYSCTL_USE_PLL );     // Use 40MHz clock

    // Enable peripheral for on-board LED (PF3, PF2, PF1) and push button SW1 (PF4)
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF )){}

    // 2. Enable timer0 peripheral
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_TIMER0 );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_TIMER0 )) {}

    // B. Peripheral level configuration
    // 3. Configure LED pins and SW1 pin
    MAP_GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );
    MAP_GPIOPinTypeGPIOInput( GPIO_PORTF_BASE , GPIO_PIN_4 );
    MAP_GPIOPadConfigSet( GPIO_PORTF_BASE , GPIO_PIN_4 , GPIO_STRENGTH_2MA , GPIO_PIN_TYPE_STD_WPU );

    // 4. Configure Timer0
    // Timer counts down once and expires.
    MAP_TimerConfigure( TIMER0_BASE , TIMER_CFG_ONE_SHOT );

    // Timer counts up once and expires.
    //MAP_TimerConfigure( TIMER0_BASE , TIMER_CFG_ONE_SHOT_UP );

    // 5. Setup timer period
    period = MAP_SysCtlClockGet() / 1000; // period == system_clock / target_frequency
    MAP_TimerLoadSet( TIMER0_BASE, TIMER_BOTH , period - 1); // Set to 1kHz

    // Alert that configuration is complete
    MAP_GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_3 , GPIO_PIN_3 );
    MAP_SysCtlDelay(800000);
    MAP_GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_3 , 0x00 );

    // Wait for SW1 to be pressed
    while( MAP_GPIOPinRead( GPIO_PORTF_BASE , GPIO_PIN_4 ) == GPIO_PIN_4 ) {}

    // 6. Enable timer
    MAP_TimerEnable( TIMER0_BASE , TIMER_BOTH );

    MAP_GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0E );
    while( MAP_TimerValueGet( TIMER0_BASE , TIMER_A ) > 0 ) {} // TimerValueGet function only takes TimerA or TimerB as second argument; for 32-bit timers, TimerA is used
    MAP_GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00 );

    while(1) {}
}
