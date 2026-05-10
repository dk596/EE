/*
 * ---------------------
 * Title: Person Detector
 * ---------------------
 * Program Details:
 * The purpose of this program is to detect a presence
 * Inputs: 
 * Outputs: 
 * Date: 5/2/26
 * File Dependencies / Libraries: It is required to include the 
 * Configuration Header File 
 * Compiler: xc8, 3.10
 * Device: PIC18F47K42
 * Author: Derek Kan
 * Code based on code by Dr. Farahmand
 * Versions:
 *      V1.0: Basic implementation 
 *      
 */


#include <xc.h> // must have this
#include "header.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4

#define RS LATD0                   /* PORTD 0 pin is used for Register Select */
#define EN LATD1                   /* PORTD 1 pin is used for Enable */
#define ldata LATB                 /* PORTB is used for transmitting data to LCD */
#define Vref 3.3 // voltage reference 

#define LCD_Port TRISB              
#define LCD_Control TRISD

int digital; // holds the digital value 
float voltage; // hold the analog value (volt))
char data[20];
float accel; // holds acceleration value
int shake_counter;
float prev_accel; //holds previous acceleration value
float accel_delta; //delta between accel and prev_accel

void LCD_Init(void);
void Timer1_Init(void);
void LCD_Command(char );
void LCD_Char(char x);
void LCD_String(const char *);
void LCD_String_xy(char ,char ,const char*);
void MSdelay(unsigned int );
void LCD_Clear(void);
int calc_dist(void);
int current_distance;

void __interrupt(irq(IRQ_IOC), base(0x0008)) DEFAULT_ISR(void){
    if (IOCCFbits.IOCCF2) {
        for (int i = 0; i < 10; i++) 
            {
                LATDbits.LD6 = 1;  //blink LED
                __delay_ms(100);
                LATDbits.LD6 = 0;
                __delay_ms(100);   
                
            }
        LATDbits.LD6 = 0;
        IOCCFbits.IOCCF2= 0; // Clear the flag after handling
    }
}

/*****************************Main Program*******************************/

void main(void)
{     
    __delay_ms(100); 
    OSCFRQbits.FRQ = 0b010; 
    ANSELB = 0x00;   // Enable B, C, D as digital
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;
    TRISA = 0xFF; //Set RA0 to input
    ANSELA = 0x00; //Set RA0 to digital
    TRISB = 0x00;   //Enable B, D as outputs, C as input
    TRISC = 0xFF;
    TRISD = 0x00;
    TRISE = 0x00;
    PORTB = 0x00;    //Clear B, D
    PORTD = 0x00;
    LATB = 0x00;
    LATD = 0x00;
    LATEbits.LE0 = 1;
    WPUCbits.WPUC2 = 0;   //Make sure pull up is off for RC2
    INTCON0bits.GIEH = 1; // Enable high priority interrupts
    INTCON0bits.GIEL = 1; // Enable low priority interrupts
    INTCON0bits.IPEN = 1; // Enable interrupt priority
    PIE0bits.IOCIE = 1; // Enable Interrupt-on-Change
    IOCCPbits.IOCCP2 = 1;     // Enable Positive Edge detection
    IOCCNbits.IOCCN2 = 1;     // Disable Negative Edge detection
    IPR0bits.IOCIP = 1;   //Enable High Priority for IOC
    IOCCFbits.IOCCF2 = 0; // Clear flag
    LCD_Init();                    // Initialize 16x2 LCD 
    Timer1_Init();
    
    __delay_ms(100);
    prev_accel = 0;
    while(1)
    {
        current_distance = calc_dist();
        sprintf(data, "%d", current_distance); 
        strcat(data," cm     ");	/*Concatenate result and unit to print*/ 
        LCD_String_xy(1,0,data);
        __delay_ms(1);
    }
  
            
}

/****************************Functions********************************/
void LCD_Init(void)
{
    
    MSdelay(15);           /* 15ms,16x2 LCD Power on delay */

    LCD_Port = 0x00;       /* Set PORTB as output PORT for LCD data(D0-D7) pins */
    LCD_Control = 0x00;    /* Set PORTD as output PORT LCD Control(RS,EN) Pins */
    LCD_Command(0x01);     /* clear display screen */
    LCD_Command(0x38);     /* uses 2 line and initialize 5*7 matrix of LCD */
    LCD_Command(0x0c);     /* display on cursor off */
    LCD_Command(0x06);     /* increment cursor (shift cursor to right) */
}


void LCD_Command(char cmd )
{
    ldata= cmd;            /* Send data to PORT as a command for LCD */   
    RS = 0;                /* Command Register is selected */
    EN = 1;                /* High-to-Low pulse on Enable pin to latch data */ 
    NOP();
    __delay_ms(1);
    EN = 0;
    MSdelay(3); 
}

void LCD_Char(char dat)
{
    ldata= dat;            /* Send data to LCD */  
    RS = 1;                /* Data Register is selected */
    EN=1;                  /* High-to-Low pulse on Enable pin to latch data */   
    NOP();
    __delay_ms(1);
    EN=0;
    MSdelay(1);
}


void LCD_String(const char *msg)
{
    while((*msg)!=0)
    {       
      LCD_Char(*msg);
      msg++;    
        }
}

void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    if(row<=1.5)
    {
        location=(0x80) | ((pos) & 0x0f); /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f); /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }  
    LCD_String(msg);

}
/*********************************Delay Function********************************/
void MSdelay(unsigned int val)
{
     unsigned int i,j;
        for(i=0;i<val;i++)
            for(j=0;j<165;j++);      /*This count Provide delay of 1 ms for 8MHz Frequency */
}





void Timer1_Init(void)
{
    T1CONbits.CKPS = 0b00;  // Prescaler 1:1
    T1CLK = 0b0001;         // Clock source: Fosc/4
    T1CONbits.ON = 1;       // Enable Timer1

}


int calc_dist(void)
{
    int distance = 0;
    
    // Reset Timer1 count
    TMR1 = 0;
    PORTA = 0x00;
    // Send 10us Trigger Pulse on RD3
    LATDbits.LATD3 = 1;      
    __delay_us(20);
    LATDbits.LATD3 = 0;

    // Wait for Echo Pulse to start (High) on RA0
    while(!PORTAbits.RA0);   
    
    // Turn ON Timer1
    T1CONbits.ON = 1;

    // Wait for Echo Pulse to end (Low) on RA0
    while(PORTAbits.RA0);
    
    // Turn OFF Timer1
    T1CONbits.ON = 0;
    
    // Calculate distance
    // (Ensure your Fosc/4 is 1MHz for this divisor to be accurate)
    //distance = TMR1 / 58; 
    distance = TMR1 / 11.6; 
    return distance;

}
