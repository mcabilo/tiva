/*
 * Name: 008_pwm-timer
 * Ver: 2023-Jan-24
 * Author: Mark Anthony Cabilo
 *
 * Description:
 * Demonstrate the use of the PWM peripheral in Tiva C Launchpad.
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
#include "driverlib/pin_map.h"      // macros for alternate pin functions
#include "driverlib/pwm.h"          // PWM API
#include "driverlib/rom_map.h"      // macros for memory-saving API calls

/**
 * MAIN FUNCTION
 */
void main(void)
{
    /**
     * Application:
     * We will use PWM to smoothly transition colors and intensity
     * of the on-board LED.
     * Three 16-bit timers will be used to individually control the
     * intensity of RGB leds.
     *
     */

    // A. System level configuration
    // 1. Setup system clock
    MAP_SysCtlClockSet( SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_5 ); // Use MOSC to drive 400MHz PLL. The use sysdiv5 to apply a /10 divisor and finally generating a 40MHz clock signal.

    // 2. Enable peripherals (Using PWM1 since it uses LED pins)
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_PWM1 );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_PWM1 )) {}
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    while(!MAP_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF )) {}

    // 3. Set PWM clock
    MAP_SysCtlPWMClockSet( SYSCTL_PWMDIV_1 );

    // B. Peripheral level configuration
    // Configure LED pins
    MAP_GPIOPinTypePWM( GPIO_PORTF_BASE , GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );
    MAP_GPIOPinConfigure( GPIO_PF1_M1PWM5 );
    MAP_GPIOPinConfigure( GPIO_PF2_M1PWM6 );
    MAP_GPIOPinConfigure( GPIO_PF3_M1PWM7 );

    // 4. Configure the PWM generator for PF1
    MAP_PWMGenConfigure( PWM1_BASE , PWM_GEN_2 , PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC ); // count down mode and no synchronization of counter load & comparator update
    MAP_PWMGenConfigure( PWM1_BASE , PWM_GEN_3 , PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC );

    // 5. Set PWM period
    // Formula: value = system_clock / target_frequency
    MAP_PWMGenPeriodSet( PWM1_BASE , PWM_GEN_2 , 800); // 40MHz / 50kHz = 800
    MAP_PWMGenPeriodSet( PWM1_BASE , PWM_GEN_3 , 800);

    // 6. Set pulse width of PF1 to 40%, PF2 to 50%, and PF3 to 10%
    MAP_PWMPulseWidthSet( PWM1_BASE , PWM_OUT_5 , 1);
    MAP_PWMPulseWidthSet( PWM1_BASE , PWM_OUT_6 , 1);
    MAP_PWMPulseWidthSet( PWM1_BASE , PWM_OUT_7 , 1);


    // 7. Enable PWM output channel
    MAP_PWMOutputState( PWM1_BASE , PWM_OUT_5_BIT , true );
    MAP_PWMOutputState( PWM1_BASE , PWM_OUT_6_BIT , true );
    MAP_PWMOutputState( PWM1_BASE , PWM_OUT_7_BIT , true );

    // 8. Enable PWM generator
    MAP_PWMGenEnable( PWM1_BASE , PWM_GEN_2 );
    MAP_PWMGenEnable( PWM1_BASE , PWM_GEN_3 );

    int8_t redDC, greenDC, blueDC;

    while(1){
        for(redDC = 1; redDC < 100; redDC++){
            greenDC = 100 - redDC;
            MAP_PWMPulseWidthSet( PWM1_BASE , PWM_OUT_5 , redDC);
            MAP_PWMPulseWidthSet( PWM1_BASE , PWM_OUT_7 , greenDC);
            MAP_SysCtlDelay(80000);
        }

        for(greenDC = 1; greenDC < 100; greenDC++){
            blueDC = 100 - greenDC;
            MAP_PWMPulseWidthSet( PWM1_BASE , PWM_OUT_6 , blueDC);
            MAP_PWMPulseWidthSet( PWM1_BASE , PWM_OUT_7 , greenDC);
            MAP_SysCtlDelay(80000);
        }

        for(blueDC = 1; blueDC < 100; blueDC++){
            redDC = 100 - blueDC;
            MAP_PWMPulseWidthSet( PWM1_BASE , PWM_OUT_5 , redDC);
            MAP_PWMPulseWidthSet( PWM1_BASE , PWM_OUT_6 , blueDC);
            MAP_SysCtlDelay(80000);
        }
    }
}
