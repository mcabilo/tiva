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
    SysCtlClockSet( SYSCTL_OSC_MAIN | SYSCTL_USE_PLL );     // Use 40MHz clock

    // 2. Enable peripheral for on-board LED (PF3, PF2, PF1) and push button SW1 (PF4)
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    while(!SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF )){}


    // B. Peripheral level configuration
    // 3. Configure LED pins
    GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );

    // 4. Configure push-button pin
    GPIOPinTypeGPIOInput( GPIO_PORTF_BASE , GPIO_PIN_4 );

    // 5. Enable pull-up resistor
    GPIOPadConfigSet( GPIO_PORTF_BASE , GPIO_PIN_4 , GPIO_STRENGTH_8MA , GPIO_PIN_TYPE_STD_WPU );

    uint32_t value = 0;
    uint8_t state = 0x01;

    while(1){
        // Take a reading before every LED blink
        value = GPIOPinRead( GPIO_PORTF_BASE , GPIO_PIN_4 );

        // Check if push-button is pressed; if it is, then toggle LED
        if ( (value & GPIO_PIN_4) == 0 ){
            state *= 2;             // red = 0x02, blue = 0x04, green = 0x08, off = 0x01
            state %= 0x0F;          // if value is 0x08 * 2 = 0x10, the value will return to 0x01
        }

        // Blink the corresponding color (state)
        GPIOPinWrite( GPIO_PORTF_BASE , state , HIGH);
        SysCtlDelay(200000);
        GPIOPinWrite( GPIO_PORTF_BASE , state , LOW);
        SysCtlDelay(200000);
    }

}
