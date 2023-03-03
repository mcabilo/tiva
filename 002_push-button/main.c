/*
 * Name: 002_push-button
 * Ver: 2023-Jan-23
 * Author: Mark Anthony Cabilo
 *
 * Description:
 * Demonstrate the use of digital input in Tiva C by using the on-board push button to
 * control the on-board LED.
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
#include "driverlib/rom_map.h"      // macros for memory-saving API calls

/**
 * MACROS
 */
#define HIGH                0x0E
#define LOW                 0x00


/**
 * MAIN FUNCTION
 */
void main(void)
{
    // A. System level configuration
    // 1. Setup system clock
    MAP_SysCtlClockSet( SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_5 ); // Use MOSC to drive 400MHz PLL. The use sysdiv5 to apply a /10 divisor and finally generating a 40MHz clock signal.

    // 2. Enable peripheral for on-board LED (PF3, PF2, PF1) and push button SW1 (PF4)
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF )){}


    // B. Peripheral level configuration
    // 3. Configure LED pins
    MAP_GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );

    // 4. Configure push-button pin
    MAP_GPIOPinTypeGPIOInput( GPIO_PORTF_BASE , GPIO_PIN_4 );

    // 5. Enable pull-up resistor
    MAP_GPIOPadConfigSet( GPIO_PORTF_BASE , GPIO_PIN_4 , GPIO_STRENGTH_8MA , GPIO_PIN_TYPE_STD_WPU );

    uint32_t value = 0;
    uint8_t state = 0x02;

    while(true){
        // Take a reading before every LED blink
        value = MAP_GPIOPinRead( GPIO_PORTF_BASE , GPIO_PIN_4 );

        // Check if push-button is pressed; if it is, then toggle LED
        if ( (value & GPIO_PIN_4) == 0 )
            state = (state != 0x08) ? state << 1 : 0x02;  // red = 0x02, blue = 0x04, green = 0x08

        // Blink the corresponding color (state)
        MAP_GPIOPinWrite( GPIO_PORTF_BASE , state , HIGH);
        MAP_SysCtlDelay(2000000);
        MAP_GPIOPinWrite( GPIO_PORTF_BASE , state , LOW);
        MAP_SysCtlDelay(2000000);
    }

}
