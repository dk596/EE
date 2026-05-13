/*
 * Title: Control Servo Motor with PWM
 * Program Details:
* The purpose of this program is to generate a PWM with different pulse width 
* to control a servo motor 
* The output of the PWM will be on RB2
* In order to change the PULSE period and width you need to do the following: 

 * PWM Period=
 * [T2PR+1]*4*Tosc*PreScale

 * Pulse Width=
 * Tosc*Prescale*CCPR2

 * Duty Cycle Ratio %=
 * CCPR2 / [4*(T2PR+1)]

 * The presale value for the timer is defined in T2CON register. 
 * The ACTUAL value of CCP2 MUST be varied by changing
 * PWM2_INITIALIZE_DUTY_VALUE set to the equivalent decimal value for CCPR2
 * Inputs: RD0, RD1
 * Outputs: RB2
 * Date: 5/12/26
 * File Dependencies / Libraries: It is required to include the 
 * Configuration Header File 
 * Compiler: xc8, 3.10
 * Device: PIC18F47K42
 * Author: Derek Kan
 * Credit to Farid Farahmand for some of the code
 * Versions:
 *      V1.0: Basic implementation 
 */




#include <xc.h> // must have this
#include "PWMheader.h" // must have this
#include "header.h" // must have this -  XC8_ConfigFile.h
//#include "../../../../../Program Files/Microchip/xc8/v2.40/pic/include/proc/pic18f46k42.h"
//#include "C:\Program Files\Microchip\xc8\v2.40\pic\include\proc\pic18f46k42"


#define _XTAL_FREQ 4000000      // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4     // system clock is 1 usec

#define myLED  PORTBbits.RB0
#define PWM2_INITIALIZE_DUTY_VALUE 31

uint16_t checkdutyCycle;
char preScale;
_Bool pwmStatus;
int PWM2_edit;
void main (void) {
    OSCSTATbits.HFOR =1; // enable  HFINTOSC Oscillator (see clock schematic))
    OSCFRQ=0x02; // 00=1 MHZ, 02=4MHZ internal - see page 106 of data sheet
    ANSELD = 0b00000000;    
    TRISD= 0b11111111; 
    WPUD = 0b11111111;
    ANSELB = 0b00000000;    
    TRISB= 0b00000000;//sets PORTB as all outputs 
    PORTB= 0b00000000;//turns off PORTB outputs so that the LED is initially off
    PORTD = 0b11111111;
    TMR2_Initialize();
    TMR2_StartTimer();        
    
    PWM_Output_D8_Enable();
    PWM2_Initialize();
    PWM2_LoadDutyValue(PWM2_INITIALIZE_DUTY_VALUE ); // initialize CCPR2H/L
    PWM2_edit = 31;
   // PWM_Output_D8_Disable();
   // TMR2_StopTimer();  

    // Duty Cycle in percentage 
    checkdutyCycle =(uint16_t)((100UL*PWM2_INITIALIZE_DUTY_VALUE)/(4*(T2PR+1)));
    // binary value of Register T2CON.PRESCALE
    preScale = ((T2CON >> 4) & (0x0F)); 
    
    while (1) {
        if (PORTDbits.RD0 == 0)
        {
            if (PWM2_edit > 15)
            {
                PWM2_edit--;
                PWM2_LoadDutyValue(PWM2_edit);
            }
        }
        __delay_ms(15); 


        if (PORTDbits.RD1 == 0)
        {
            if (PWM2_edit < 75)
            {
                PWM2_edit++;
                PWM2_LoadDutyValue(PWM2_edit);
            }
        }
        __delay_ms(15); 
    }
}
