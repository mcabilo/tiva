

/**
 * Name: 001_first-program_blink
 * Ver: 2023-Jan-23
 * Author: Mark Anthony Cabilo
 *
 * Description:
 * A sample code demonstrating Tiva C built-in LED blinking
 */

/*
 * HEADER FILES
 */
#include "stdint.h"                 // standard integer definitions (e.g., uint32_t, uint16_t, etc.)
#include "stdbool.h"                // standard boolean definitions
#include "inc/hw_memmap.h"          // hardware memory map macros
#include "driverlib/sysctl.h"       // system control API
#include "driverlib/gpio.h"         // general-purpose input output API
#include "driverlib/rom_map.h"      // a memory-saving programming model for all APIs

/*
 * MACROS
 */
#define HIGH        0x0E
#define LOW         0x00
#define RED         GPIO_PIN_1
#define BLUE        GPIO_PIN_2
#define GREEN       GPIO_PIN_3
#define YELLOW      GPIO_PIN_1 | GPIO_PIN_3
#define CYAN        GPIO_PIN_2 | GPIO_PIN_3
#define PURPLE      GPIO_PIN_1 | GPIO_PIN_2
#define WHITE       GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3

/*
 * MAIN FUNCTION
 */
void main(void)
{
    /**
     * Notes:
     *
     * a. System Control (SysCtl) manages MCU peripherals including clocks
     *
     * b. General Purpose I/O (GPIO) is a peripheral that controls all MCU I/O pins; one of its simplest application is to perform digital output
     */

    // 1. Configure system clock
    MAP_SysCtlClockSet( SYSCTL_USE_PLL | SYSCTL_OSC_MAIN ); //main osc = 1MHz |  Div 5 = 20MHz

    // 2. Enable a GPIO port (GPIO A-F)
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF )){} // wait until peripheral is actually enabled

    // 3. Configure GPIO pins as output
    MAP_GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 ); // (PF1, PF2, and PF3 control the built-in RGB LED)


    // Proceed with whatever function you want your configured pins to do...
    while(1) {
        MAP_GPIOPinWrite( GPIO_PORTF_BASE , WHITE , HIGH);
        MAP_SysCtlDelay(200000);
        MAP_GPIOPinWrite( GPIO_PORTF_BASE , WHITE , LOW);
        MAP_SysCtlDelay(200000);

        MAP_GPIOPinWrite( GPIO_PORTF_BASE , RED , HIGH);
        MAP_SysCtlDelay(200000);
        MAP_GPIOPinWrite( GPIO_PORTF_BASE , RED , LOW);
        MAP_SysCtlDelay(200000);

        MAP_GPIOPinWrite( GPIO_PORTF_BASE , YELLOW , HIGH);
        MAP_SysCtlDelay(200000);
        MAP_GPIOPinWrite( GPIO_PORTF_BASE , YELLOW , LOW);
        MAP_SysCtlDelay(200000);
    }

}
