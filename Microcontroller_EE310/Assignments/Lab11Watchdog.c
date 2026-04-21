/*
 * File:   main.c
 * Author: derek
 *
 * Created on April 21, 2026, 2:12 PM
 */

#define BlinkLED PORTDbits.RD0
#define WdtLED PORTDbits.RD1
#define PORTD_DIR TRISD
#define OUTPUT 0
#include <xc.h>
#include "header.h"
#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4
void Dummy_Stuck(void);

void main(void) {
    ANSELD = 0b00000000; // set port B and D as outputs 
    TRISD = 0b00000000; 
    PORTD = 0b00000000; 
    
    
    __delay_ms(1000);
    int count = 0;
    BlinkLED = 0;
    WdtLED = 0;
    while(1)
    {
        BlinkLED = !BlinkLED;
        __delay_ms(100);
        CLRWDT();
        count++;
        if(count>5)
        {
            WdtLED = 1;
            Dummy_Stuck();
            
        }
    }
    return;
}

void Dummy_Stuck(void)
{
    while(1);
}
