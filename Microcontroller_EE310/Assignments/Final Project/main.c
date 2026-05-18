/*
 * ---------------------
 * Title: Touchless Lever
 * ---------------------
 * Program Details:
 * The purpose of this program is to control a servo motor using an
 * ultrasonic sensor
 * Inputs: RA0, RA1
 * Outputs: PORTB, RC2, RD0, RD1, RD3, RD6
 * Date: 5/18/26
 * File Dependencies / Libraries: It is required to include the 
 * Configuration Header File and function file 
 * Compiler: xc8, 3.10
 * Device: PIC18F47K42
 * Author: Derek Kan
 * Code based on code by Dr. Farahmand
 * Versions:
 *      V1.0: Basic implementation 
 *      V1.1: Added servo motor
 *      V1.2: Cleaned up code and comments
 */


#include <xc.h> // must have this
#include "header.h"
#include "functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4

char data[20];
int current_distance;
float smoothedDistance = 0.0;
float alpha = 0.3; // Smoothing factor (Between 0.0 and 1.0)
                   // Lower = smoother but slower response. Higher = faster but twitchier.

void __interrupt(irq(IRQ_IOC), base(0x0008)) DEFAULT_ISR(void){
    if (IOCAFbits.IOCAF1) {
        for (int i = 0; i < 10; i++) 
            {
                LATDbits.LD6 = 1;  //blink LED
                __delay_ms(100);
                LATDbits.LD6 = 0;
                __delay_ms(100);   
                
            }
        LATDbits.LD6 = 0;
        IOCAFbits.IOCAF1 = 0; // Clear the flag after handling
    }
}

/*****************************Main Program*******************************/

void main(void)
{     
    
    OSCFRQbits.FRQ = 0b010;  
    while(OSCSTATbits.HFOR == 0); 
    __delay_ms(100); 
    ANSELA = 0x00; //all digital
    ANSELB = 0x00;   
    ANSELC = 0x00;
    ANSELD = 0x00;
    TRISA = 0xFF; //PORTA as input
    TRISB = 0x00;   //Enable B, C, D as outputs
    TRISC = 0x00;
    TRISD = 0x00;

    PORTB = 0x00;    //Clear B, C, D
    PORTC = 0x00;
    PORTD = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;

    WPUAbits.WPUA1 = 1;   //Make sure pull up is on for RA1
    INTCON0bits.GIEH = 1; // Enable high priority interrupts
    INTCON0bits.GIEL = 1; // Enable low priority interrupts
    INTCON0bits.IPEN = 1; // Enable interrupt priority
    PIE0bits.IOCIE = 1; // Enable Interrupt-on-Change
    IOCANbits.IOCAN1 = 1;      // Enable Positive Edge detection
    IOCAPbits.IOCAP1 = 1;     // Disable Negative Edge detection
    IPR0bits.IOCIP = 1;   //Enable High Priority for IOC
    IOCAFbits.IOCAF1 = 0; // Clear flag
    LCD_Init();                    // Initialize 16x2 LCD 
    Timer1_Init(); //Initialize Timer1
    TMR2_Initialize();
    TMR2_StartTimer();  
    PWM_Output_Enable();
    PWM2_Initialize();
    __delay_ms(100);
    
    while(1)
    {
        uint16_t readings[3];
        readings[0] = calc_dist();
        __delay_ms(15);
        readings[1] = calc_dist();
        __delay_ms(15);
        readings[2] = calc_dist();
        if (readings[0] > readings[1]) { int temp = readings[0]; readings[0] = readings[1]; readings[1] = temp; }
        if (readings[1] > readings[2]) { int temp = readings[1]; readings[1] = readings[2]; readings[2] = temp; }
        if (readings[0] > readings[1]) { int temp = readings[0]; readings[0] = readings[1]; readings[1] = temp; }
        
        
        float rawDistance = readings[1];
            // First run initialization
        if (smoothedDistance == 0.0) 
        {
           smoothedDistance = rawDistance;
        }
        smoothedDistance = (alpha * rawDistance) + ((1.0 - alpha) * smoothedDistance);
        
        current_distance = smoothedDistance;
        sprintf(data, "%d", readings[1]); 
        strcat(data," cm     ");	/*Concatenate result and unit to print*/ 
        LCD_String_xy(1,0,data);
        if (current_distance < 50)
        {
            PWM2_LoadDutyValue(current_distance + 20);
            
        }
        else
        {
            PWM2_LoadDutyValue(75);
        }
        
        __delay_ms(15);
    }
  
            
}
