/**
 * Name: 003_gpio-interrupt
 * Ver: 2023-Jan-24
 * Author: Mark Anthony Cabilo
 *
 * Description:
 * Demonstrates the use of GPIO interrupt by creating a no-delay blinking with quick response color change
 */

/**
 * HEADER FILES
 */
#include "stdint.h"                     // standard integer library
#include "stdbool.h"                    // standard boolean library
#include "inc/hw_memmap.h"              // hardware memory map macros
#include "inc/tm4c123gh6pm.h"           // board-specific macros
#include "driverlib/sysctl.h"           // system control API
#include "driverlib/gpio.h"             // general-purpose IO API
#include "driverlib/interrupt.h"        // system interrupt API

/**
 * MACROS
 */
#define HIGH                0x0E
#define LOW                 0x00

/**
 * GLOBAL VARIABLES
 */
uint8_t state = 0x02;   // LED state

/**
 * ISR
 */
void toggle_color(void){
    // Always clear interrupt flags when interrupt handler is successfully called to allow next interrupt to trigger
    GPIOIntClear( GPIO_PORTF_BASE , GPIO_PIN_4 );

    // Update values
    if (state == 0x08)
        state = 0x02;
    else
        state *= 2;
}


/**
 * MAIN FUNCTION
 */
void main(void)
{
    // A. System control level configuration
	// 1. Configure system clock
    SysCtlClockSet( SYSCTL_OSC_MAIN | SYSCTL_USE_PLL ); // clock set to 40MHz

    // 2. Enable GPIOF peripheral
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    while(!SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF )) {}

    // B. Peripheral level configuration
    // 3. Configure LED pins as output
    GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );

    // 4. Configure SW1 as input
    GPIOPinTypeGPIOInput( GPIO_PORTF_BASE , GPIO_PIN_4 );

    // 5. Configure pull-up resistor for SW1
    GPIOPadConfigSet( GPIO_PORTF_BASE , GPIO_PIN_4, GPIO_STRENGTH_2MA , GPIO_PIN_TYPE_STD_WPU );

    // 6. Register the port-level interrupt service routine (interrupt handler).
    GPIOIntRegister( GPIO_PORTF_BASE , toggle_color );

    // 7. Configure SW1 interrupt (triggered when button is pressed down = shorted to ground)
    GPIOIntTypeSet( GPIO_PORTF_BASE , GPIO_PIN_4 , GPIO_FALLING_EDGE );

    // 8. Enable the pin interrupts
    GPIOIntEnable( GPIO_PORTF_BASE , GPIO_PIN_4 );

    // C. System interrupt configuration
    // 9. Set interrupts' priorities (optional); without this line, interrupts follow their default priorities (see board-specific macro library)
    IntPrioritySet( INT_GPIOF , 0 );

    // 10. Enable interrupt from peripheral
    IntEnable( INT_GPIOF );

    // 11. Enable interrupts to the processor
    IntMasterEnable();


    while(1){
        GPIOPinWrite( GPIO_PORTF_BASE , state , HIGH );
        SysCtlDelay(200000);
        GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 , LOW );
        SysCtlDelay(200000);
    }
}
