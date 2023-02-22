/*
 * Name: 009_basic-adc
 * Ver: 2023-Feb-18
 * Author: Mark Anthony Cabilo
 *
 * Description:
 * Demonstrate the use of the ADC peripheral in Tiva C Launchpad.
 *
 */


/**
 * HEADER FILES
 */
#include "stdint.h"                 // standard integer library
#include "stdbool.h"                // standard boolean library
#include "inc/hw_memmap.h"          // macros for memory map
#include "driverlib/sysctl.h"       // system control API
#include "driverlib/gpio.h"         // general purpose input output API
#include "driverlib/adc.h"          // analog-to-digital converter API
#include "driverlib/rom_map.h"      // macros for memory-saving API calls


/**
 * MAIN FUNCTION
 */
void main(void)
{

    /**
     * Application:
     * We will enable an ADC channel that can sample signals
     * through a simple function call (ADC processor trigger)
     *
     */

    // variable to store ADC result
    uint32_t ui32AdcOut;


    // A. System level configuration
    // 1. Setup system clock
    MAP_SysCtlClockSet( SYSCTL_OSC_MAIN | SYSCTL_USE_OSC );     // Use 40MHz clock

    // 2. Enable GPIO first (ADC channel 0 is located at PE3)
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );
    while( !MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOE )) {}

    // 3. Enable the ADC0 module
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!MAP_SysCtlPeripheralReady (SYSCTL_PERIPH_ADC0)){}


    // B. Peripheral level configuration
    // 4. Configure PE3 to use its ADC function
    MAP_GPIOPinTypeADC( GPIO_PORTE_BASE, GPIO_PIN_3 );

    // 5. Enable the first sample sequencer to capture the value of channel 0 when the processor trigger occurs
    MAP_ADCSequenceConfigure( ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0 );
    MAP_ADCSequenceStepConfigure( ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0 );
    MAP_ADCSequenceEnable( ADC0_BASE, 0 );

    while(1) {
        // 6. This function initiates 1 conversion
        MAP_ADCProcessorTrigger( ADC0_BASE, 0 );
        while( !MAP_ADCIntStatus( ADC0_BASE, 0, false ));

        // 7. Save sampled data into a variable
        MAP_ADCSequenceDataGet( ADC0_BASE, 0, &ui32AdcOut );
    }

}
