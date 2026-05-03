/*
 * ---------------------
 * Title: Read Accelerometer on LCD
 * ---------------------
 * Program Details:
 * The purpose of this program is to read an accelerometer and output
 * acceleration to 16x2 LCD
 * Inputs: RC2, RC3, RC4
 * Outputs: PORTD, RB0, RB3
 * Date: 5/2/26
 * File Dependencies / Libraries: It is required to include the 
 * Configuration Header File 
 * Compiler: xc8, 3.10
 * Device: PIC18F47K42
 * Author: Derek Kan
 * Code based on code by Dr. Farahmand
 * Versions:
 *      V1.0: Basic implementation 
 *      V1.1: Added comments
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
char data[10];
float accel; // holds acceleration value
int shake_counter;
float prev_accel; //holds previous acceleration value
float accel_delta; //delta between accel and prev_accel
void ADC_Init(void);
void LCD_Init(void);
void LCD_Command(char );
void LCD_Char(char x);
void LCD_String(const char *);
void LCD_String_xy(char ,char ,const char*);
void MSdelay(unsigned int );
void LCD_Clear(void);


void __interrupt(irq(IRQ_IOC), base(0x0008)) DEFAULT_ISR(void){
    if (IOCCFbits.IOCCF2) {
        for (int i = 0; i < 5; i++) 
            {
                LATDbits.LD6 = 1;  //blink LED
                __delay_ms(250);
                LATDbits.LD6 = 0;
                __delay_ms(250);   
                
            }
        LATDbits.LD6 = 0;
        IOCCFbits.IOCCF2= 0; // Clear the flag after handling
    }
}

/*****************************Main Program*******************************/

void main(void)
{     
    __delay_ms(100);   
    ANSELB = 0x00;   // Enable B, C, D as digital
    ANSELC = 0x00;
    ANSELD = 0x00;
    TRISB = 0x00;   //Enable B, D as outputs, C as input
    TRISC = 0xFF;
    TRISD = 0x00;
    PORTB = 0x00;    //Clear B, D
    PORTD = 0x00;
    LATB = 0x00;
    LATD = 0x00;
    WPUCbits.WPUC2 = 0;   //Make sure pull up is off for RC2
    INTCON0bits.GIEH = 1; // Enable high priority interrupts
    INTCON0bits.GIEL = 1; // Enable low priority interrupts
    INTCON0bits.IPEN = 1; // Enable interrupt priority
    PIE0bits.IOCIE = 1; // Enable Interrupt-on-Change
    IOCCPbits.IOCCP2 = 1;     // Enable Positive Edge detection
    IOCCNbits.IOCCN2 = 1;     // Disable Negative Edge detection
    IPR0bits.IOCIP = 1;   //Enable High Priority for IOC
    IOCCFbits.IOCCF2 = 0; // Clear flag
    ADC_Init();                 // Initialize ADC
    LCD_Init();                    // Initialize 16x2 LCD 
    __delay_ms(100);
    prev_accel = 0;

    while(1)
    {
        ADCON0bits.GO = 1; //Start conversion
        while (ADCON0bits.GO); //Wait for conversion done
        digital = (ADRESH*256) | (ADRESL);/*Combine 8-bit LSB and 2-bit MSB*/
        voltage= digital*((float)Vref/(float)(4095)); 
        accel=((voltage-(1.66))/0.3)*9.81;
        //print on LCD 
        /*It is used to convert integer value to ASCII string*/
        sprintf(data,"%.2f",accel);

        strcat(data," m/s^2 ");	/*Concatenate result and unit to print*/  
        
        accel_delta = abs(accel - prev_accel);
        prev_accel = accel;
        if (accel_delta > 3)
        {
            shake_counter++; //multiple high deltas in a row is a shake
            if (shake_counter >= 3)
            {
                LCD_String_xy(1,0,"Shake!           "); 
                LCD_String_xy(2,0,data);
                __delay_ms(1000); //display shake for a longer time
                continue;
            }
            LCD_String_xy(2,0,data);
            __delay_ms(20);
            continue;
        }
        else if (accel > 2)
        {
            LCD_String_xy(1,0,"Tilt Left       "); 
        }
        else if (accel < 0)
        {
            LCD_String_xy(1,0,"Tilt Right      "); 
        }    
        else
        {
            LCD_String_xy(1,0,"Level           ");    
        }
        shake_counter = 0;  
        LCD_String_xy(2,0,data);   /*Display string at location(row,location). */
                                   /* This function passes string to display */

        __delay_ms(20);
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


void ADC_Init(void)
{
       //Setup ADC
    ADCON0bits.FM = 1;  //right justify
    ADCON0bits.CS = 1; //ADCRC Clock
    
    TRISAbits.TRISA0 = 1; //Set RA0 to input
    ANSELAbits.ANSELA0 = 1; //Set RA0 to analog

    // Added 
    ADPCH = 0x00; //Set RA0 as Analog channel in ADC ADPCH
    ADCLK = 0x00; //set ADC CLOCK Selection register to zero
    
    ADRESH = 0x00; // Clear ADC Result registers
    ADRESL = 0x00; 
    
    ADPREL = 0x00; // set precharge select to 0 in register ADPERL & ADPERH
    ADPREH = 0x00; 
    
    ADACQL = 0x00;  // set acquisition low and high byte to zero 
    ADACQH = 0x00;    
    
    ADCON0bits.ON = 1; //Turn ADC On 
}
