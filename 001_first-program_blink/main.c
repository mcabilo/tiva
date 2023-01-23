

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
#include "inc/hw_types.h"           // for translating hw register formats to their sw equivalent
#include "inc/hw_memmap.h"          // device memory map macros
#include "driverlib/sysctl.h"       // system control API
#include "driverlib/gpio.h"         // general-purpose input output API

/*
 * MACROS
 */
#define HIGH        255
#define LOW         0
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
    SysCtlClockSet( SYSCTL_USE_PLL | SYSCTL_OSC_MAIN ); //main osc = 1MHz |  Div 5 = 20MHz

    // 2. Enable a GPIO port (GPIO A-F)
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    while(!SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF )){} // wait until peripheral is actually enabled

    // 3. Configure GPIO pins as output
    GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 ); // (PF1, PF2, and PF3 control the built-in RGB LED)


    // Proceed with whatever function you want your configured pins to do...
    while(1) {
        GPIOPinWrite( GPIO_PORTF_BASE , WHITE , HIGH);
        SysCtlDelay(200000);
        GPIOPinWrite( GPIO_PORTF_BASE , WHITE , LOW);
        SysCtlDelay(200000);

        GPIOPinWrite( GPIO_PORTF_BASE , RED , HIGH);
        SysCtlDelay(200000);
        GPIOPinWrite( GPIO_PORTF_BASE , RED , LOW);
        SysCtlDelay(200000);

        GPIOPinWrite( GPIO_PORTF_BASE , YELLOW , HIGH);
        SysCtlDelay(200000);
        GPIOPinWrite( GPIO_PORTF_BASE , YELLOW , LOW);
        SysCtlDelay(200000);
    }

}
