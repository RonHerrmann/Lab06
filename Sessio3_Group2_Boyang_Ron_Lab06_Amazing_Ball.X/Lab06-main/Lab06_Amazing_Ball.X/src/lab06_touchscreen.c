#include "lab06_touchscreen.h"

#include <xc.h>
//do not change the order of the following 2 definitions
#define FCY 12800000UL
#include <libpic30.h>

#include "types.h"
#include "lcd.h"

#define X 0
#define Y 1

/*
 * Function to initilaize ADC1
 */

void ADC1_initialization(){
    
    //init ADC1
    
    CLEARBIT(AD1CON1bits.ADON); // disable ADC1
    
    SETBIT(TRISBbits.TRISB15); // set AN15 as input for x-Axis
    SETBIT(TRISBbits.TRISB9); // set AN9 as input for y-Axis
    CLEARBIT(AD1PCFGLbits.PCFG15); // set AD1 AN15 input pin as analog
    CLEARBIT(AD1PCFGLbits.PCFG9); // set AD1 AN9 input pin as analaog
    
    //Configurate AD1CON1
    CLEARBIT(AD1CON1bits.AD12B); // set 10b Operation Mode 1 CHANNEL
    AD1CON1bits.FORM = 0; // set iunteger output
    AD1CON1bits.SSRC = 0x7; // set automatic conversion
    
    //Configure AD1CON2
    AD1CON2 = 0; // not using scanning sampling
    
    //Configure AD1CON3
    CLEARBIT(AD1CON3bits.ADRC); // intrnal clock source
    AD1CON3bits.SAMC = 0x1F; // sample-to-conversion clock = 31 Tad
    AD1CON3bits.ADCS = 0x2; // Tad = 3Tcy (Time cycles)
    
    //Leave AD1CON4 at its default value
    
    SETBIT(AD1CON1bits.ADON); // enable ADC1
    
}

/*
 * Function to initialize touchscreen inclusive the call to initialize_ADC1()
 */	
	

void initialize_touchscreen(){
    
    //set up the I/O pins E1, E2, E3 to be output pins
    
    CLEARBIT(TRISEbits.TRISE1); //E1 pin set to output
    Nop();
    CLEARBIT(TRISEbits.TRISE2); //E2 pin set to output
    Nop();
    CLEARBIT(TRISEbits.TRISE3); //E3 pin set to output
    Nop();
    
    SETBIT(PORTEbits.RE1);
    SETBIT(PORTEbits.RE2);
    SETBIT(PORTEbits.RE3);
    
    ADC1_initialization();
}


/*
 * Function to change the dimension (x- or y-Axis) of touchscreen
 * 
 * input: dimension x or y
 */


void changeDimension_touchscreen(uint8_t dimension){
    
    
    if(dimension == X){  // x-Axis
        CLEARBIT(PORTEbits.RE1);   
        Nop();
        SETBIT(PORTEbits.RE2);
        Nop();
        SETBIT(PORTEbits.RE3);
        __delay_ms(15);    // wait until touchscreen operation is stable
    
        AD1CHS0bits.CH0SA = 0x0F; // set ADC1 to sample AN15 pin
    } 

    if(dimension == Y){  // y-Axis
        SETBIT(PORTEbits.RE1);
        Nop();
        CLEARBIT(PORTEbits.RE2);
        Nop();
        CLEARBIT(PORTEbits.RE3);
        __delay_ms(15); // wait until touchscreen operation is stable
        
        AD1CHS0bits.CH0SA = 0x09; // set ADC1 to sample AN15 pin
        
    }
}

/*
 * Function to get the current coordinate of x- or y-Axis
 * 
 * return value of coordinate
 */

uint16_t currentBallPosition(){
    
    SETBIT(AD1CON1bits.SAMP); // start to sample
    while(!AD1CON1bits.DONE); // wait for conversion done bit
    CLEARBIT(AD1CON1bits.DONE); // MUST HAVE! clear conversion done bit
    return ADC1BUF0;
}

