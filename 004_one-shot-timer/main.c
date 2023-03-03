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
#include "inc/tm4c123gh6pm.h"       // board-specific macros
#include "driverlib/sysctl.h"       // system control API
#include "driverlib/gpio.h"         // general-purpose IO API
#include "driverlib/timer.h"        // timer API
#include "driverlib/interrupt.h"    // interrupt API
#include "driverlib/rom_map.h"      // macros for memory-saving API calls

bool isReadyToTurnOff = false;

/**
 * ISR
 */
void timerExpired() {
    MAP_TimerIntClear( TIMER0_BASE , TIMER_BOTH );
    isReadyToTurnOff = true;
}

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
    MAP_GPIOPinTypeGPIOInput( GPIO_PORTF_BASE , GPIO_PIN_4 );
    MAP_GPIOPadConfigSet( GPIO_PORTF_BASE , GPIO_PIN_4 , GPIO_STRENGTH_2MA , GPIO_PIN_TYPE_STD_WPU );

    // 4. Configure Timer0
    // Timer counts down once and expires.
    MAP_TimerConfigure( TIMER0_BASE , TIMER_CFG_PERIODIC );

    // Timer counts up once and expires.
    //MAP_TimerConfigure( TIMER0_BASE , TIMER_CFG_ONE_SHOT_UP );

    // 5. Setup timer period
    period = MAP_SysCtlClockGet() / 2;
    MAP_TimerLoadSet( TIMER0_BASE, TIMER_BOTH , period - 1); // since 1 clock cycle is 1/40MHz long, this should take 1/2 sec to finish

    // 6. Register the peripheral-level interrupt handler
    TimerIntRegister( TIMER0_BASE , TIMER_BOTH , timerExpired );

    // 7. Enable timer interrupt
    MAP_TimerIntEnable( TIMER0_BASE , TIMER_TIMA_TIMEOUT ); // use timerA for 32-bit timer timeout

    // C. System level interrupt
    // 8. Set timer interrupt priority
    MAP_IntPrioritySet( INT_TIMER0A , 0 );

    // 9. Enable interrupt from peripheral
    MAP_IntEnable( INT_TIMER0A );

    // 10. Enable interrupts to the processor
    MAP_IntMasterEnable();

    // Alert that configuration is complete
    MAP_GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_3 , GPIO_PIN_3 );
    MAP_SysCtlDelay(800000);
    MAP_GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_3 , 0x00 );

    // Wait for SW1 to be pressed
    while( MAP_GPIOPinRead( GPIO_PORTF_BASE , GPIO_PIN_4 ) == GPIO_PIN_4 ) {}

    // 6. Enable timer
    MAP_TimerEnable( TIMER0_BASE , TIMER_BOTH );

    MAP_GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0E );

    while(1) {
        if (isReadyToTurnOff){
            MAP_GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00 );
            isReadyToTurnOff = false;
        }

    }
}
