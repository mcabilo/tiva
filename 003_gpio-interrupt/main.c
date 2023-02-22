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
#include "driverlib/rom_map.h"          // macros for memory-saving API calls

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
    MAP_GPIOIntClear( GPIO_PORTF_BASE , GPIO_PIN_4 );

    // Update values
    if (state == 0x08)
        state = 0x02;
    else
        state <<= 1;
}


/**
 * MAIN FUNCTION
 */
void main(void)
{
    // A. System control level configuration
	// 1. Configure system clock
    MAP_SysCtlClockSet( SYSCTL_OSC_MAIN | SYSCTL_USE_PLL ); // clock set to 40MHz

    // 2. Enable GPIOF peripheral
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF )) {}

    // B. Peripheral level configuration
    // 3. Configure LED pins as output
    MAP_GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );

    // 4. Configure SW1 as input
    MAP_GPIOPinTypeGPIOInput( GPIO_PORTF_BASE , GPIO_PIN_4 );

    // 5. Configure pull-up resistor for SW1
    MAP_GPIOPadConfigSet( GPIO_PORTF_BASE , GPIO_PIN_4, GPIO_STRENGTH_2MA , GPIO_PIN_TYPE_STD_WPU );

    // 6. Register the port-level interrupt service routine (interrupt handler).
    MAP_GPIOIntRegister( GPIO_PORTF_BASE , toggle_color );

    // 7. Configure SW1 interrupt (triggered when button is pressed down = shorted to ground)
    MAP_GPIOIntTypeSet( GPIO_PORTF_BASE , GPIO_PIN_4 , GPIO_FALLING_EDGE );

    // 8. Enable the pin interrupts
    MAP_GPIOIntEnable( GPIO_PORTF_BASE , GPIO_PIN_4 );

    // C. System interrupt configuration
    // 9. Set interrupts' priorities (optional); without this line, interrupts follow their default priorities (see board-specific macro library)
    MAP_IntPrioritySet( INT_GPIOF , 0 );

    // 10. Enable interrupt from peripheral
    MAP_IntEnable( INT_GPIOF );

    // 11. Enable interrupts to the processor
    MAP_IntMasterEnable();


    while(true){
        MAP_GPIOPinWrite( GPIO_PORTF_BASE , state , HIGH );
        MAP_SysCtlDelay(2000000);
        MAP_GPIOPinWrite( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 , LOW );
        MAP_SysCtlDelay(2000000);
    }
}
