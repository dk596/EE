/* 
 * File:   functions.h
 * Author: Derek Kan, credit to Dr. Farahmand for base code
 *
 */

#ifndef PWM_H
#define	PWM_H

#ifdef	__cplusplus
extern "C" {
#endif

#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4
#define RS LATD0                   /* PORTD 0 pin is used for Register Select */
#define EN LATD1                   /* PORTD 1 pin is used for Enable */
#define ldata LATB                 /* PORTB is used for transmitting data to LCD */


#define LCD_Port TRISB              
#define LCD_Control TRISD

#ifdef	__cplusplus
}
#endif

#endif	/* PWM_H */



///////////////  TIMER 2
void TMR2_Initialize(void)
{
    // Set TMR2 to the options selected in the User Interface

    // T2CS FOSC/4; 
    T2CLKCON = 0x01;

    // T2PSYNC Not Synchronized; T2MODE Software control; T2CKPOL Rising Edge; T2CKSYNC Not Synchronized; 
    T2HLT = 0x00;

    // T2RSEL T2CKIPPS pin; 
    T2RST = 0x00;

    // PR2; 
    T2PR = 156;

    // TMR2 0; 
    T2TMR = 0x00;

    // Clearing IF flag.
    PIR4bits.TMR2IF = 0;

    T2CONbits.CKPS = 0b111; //1:128 scale
}



void TMR2_Start(void)
{
    // Start the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 1;
}

void TMR2_StartTimer(void)
{
    TMR2_Start();
}

void TMR2_Stop(void)
{
    // Stop the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 0;
}

void TMR2_StopTimer(void)
{
    TMR2_Stop();
}

uint8_t TMR2_Counter8BitGet(void)
{
    uint8_t readVal;

    readVal = TMR2;

    return readVal;
}

uint8_t TMR2_ReadTimer(void)
{
    return TMR2_Counter8BitGet();
}

void TMR2_Counter8BitSet(uint8_t timerVal)
{
    // Write to the Timer2 register
    TMR2 = timerVal;
}

void TMR2_WriteTimer(uint8_t timerVal)
{
    TMR2_Counter8BitSet(timerVal);
}

void TMR2_Period8BitSet(uint8_t periodVal)
{
   PR2 = periodVal;
}

void TMR2_LoadPeriodRegister(uint8_t periodVal)
{
   TMR2_Period8BitSet(periodVal);
}



void PWM_Output_Enable (void){
    TRISCbits.TRISC2 = 1;
    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS

    // Set C2 as the output of CCP2
    RC2PPS = 0x0A;

    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
    TRISCbits.TRISC2 = 0;
}

void PWM_Output_Disable (void){
    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS

    // Set C2 as GPIO pin
    RC2PPS = 0x0A;

    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
    
    TRISBbits.TRISB3 = 0;
}

void PWM2_Initialize(void)
{
    // Set the PWM2 to the options selected in the User Interface
	
	// MODE PWM; EN enabled; FMT left_aligned; 
	CCP2CON = 0x8C;  // enable / Right-aligned format  
	
	// RH 127; 
	CCPR2H = 0x0;    
	
	// RL 192; 
	CCPR2L = 0x0f;    

	// Selecting Timer 2
	CCPTMRS0bits.C2TSEL = 0x1;
    
}
void PWM2_LoadDutyValue(uint16_t dutyValue)
{
    dutyValue &= 0x03FF;

    // Load duty cycle value /Right-aligned format by default FMT=0
    if(CCP2CONbits.FMT)
    {
        dutyValue <<= 6;
        CCPR2H = dutyValue >> 8;
        CCPR2L = dutyValue;
    }
    else
    {
        CCPR2H = dutyValue >> 8;
        CCPR2L = dutyValue;
    }
}

 _Bool PWM2_OutputStatusGet(void)
{
    // Returns the output status
    return(CCP2CONbits.OUT);
}
 
void LCD_Command(char cmd )
{
    ldata= cmd;            /* Send data to PORT as a command for LCD */   
    RS = 0;                /* Command Register is selected */
    EN = 1;                /* High-to-Low pulse on Enable pin to latch data */ 
    NOP();
    EN = 0;
    __delay_ms(3);
}

void LCD_Init(void)
{
    
    __delay_ms(15);          /* 15ms,16x2 LCD Power on delay */
    LCD_Port = 0x00;       /* Set PORTB as output PORT for LCD data(D0-D7) pins */
    LCD_Control = 0x00;    /* Set PORTD as output PORT LCD Control(RS,EN) Pins */
    LCD_Command(0x01);     /* clear display screen */
    LCD_Command(0x38);     /* uses 2 line and initialize 5*7 matrix of LCD */
    LCD_Command(0x0c);     /* display on cursor off */
    LCD_Command(0x06);     /* increment cursor (shift cursor to right) */
}




void LCD_Char(char dat)
{
    ldata= dat;            /* Send data to LCD */  
    RS = 1;                /* Data Register is selected */
    EN=1;                  /* High-to-Low pulse on Enable pin to latch data */   
    NOP();
    EN=0;
    __delay_ms(1);  
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


void Timer1_Init(void)
{
    T1CONbits.CKPS = 0b00;  // Prescaler 1:1
    T1CLK = 0b0001;         // Clock source: Fosc/4
    T1CONbits.ON = 1;       // Enable Timer1

}


int calc_dist(void)
{
    int distance = 0;
    uint16_t timeout = 0;
    // Reset Timer1 count
    TMR1 = 0;
    PORTA = 0x00;
    // Send at least 10us Trigger Pulse
    LATDbits.LATD3 = 1;      
    __delay_us(12);
    LATDbits.LATD3 = 0;

    // Wait for Echo Pulse to start (High) on RA0
    while(!PORTAbits.RA0)
    {
        timeout++;
        if(timeout > 5000) 
        {
            return -1.0; // Echo failed to start! Exit immediately to prevent freezing.
        }        
    }
    timeout = 0;
    // Turn ON Timer1
    T1CONbits.ON = 1;

    // Wait for Echo Pulse to end (Low) on RA0

    while(PORTAbits.RA0)
    {
        timeout++;
        if(timeout > 5000) 
        {
            return -1.0; // Echo failed to start! Exit immediately to prevent freezing.
        }   
    }
    timeout = 0;
    // Turn OFF Timer1
    T1CONbits.ON = 0;
    
    // Calculate distance
    //distance = TMR1 / 58;
    if (TMR1 <= 10000)
    {
       distance = TMR1 / 58;  
    }
    
    return distance;

}
