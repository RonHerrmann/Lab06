#include "lab06.h"
#include "lab06_servos.h"
#include "lab06_touchscreen.h"

#include <xc.h>
//do not change the order of the following 2 definitions
#define FCY 12800000UL
#include <libpic30.h>
#include <stdint.h>

#include <math.h>

#include "types.h"
#include "lcd.h"
#include "led.h"


/*
 * Parameter
 */

#define X 0
#define Y 1
#define x_Axis 0
#define y_Axis 1
#define Null_degree 0.9
#define Ninety_degree 1.5
#define HundredEighty_degree 2.1


/*
 * Common Definitions
 */
#define TCKPS_1   0x00
#define TCKPS_8   0x01
#define TCKPS_64  0x02
#define TCKPS_256 0x03



/*
 * Global Variables
 */



/*
 * Timer Code
 */

void initialize_timer(){
    

/*****setup Timer 1 *****/
    
 T1CONbits.TON = 0; // Disable the Timers
 T1CONbits.TCKPS = 0b10; // Set Prescaler 64
 T1CONbits.TCS = 0; // Set Clock Source (external = 1)
 T1CONbits.TGATE = 0; // Set Gated Timer Mode -> don't use gating //this line can be ignored, if TCS =  1 (have a look at the manual)  
 T1CONbits.TSYNC = 1; 
 PR1 = 2000; // Set timer period 10ms (100 Hz):    // 2000= 10*10^-3 * 12.8*10^6 * 1/64 
 TMR1 = 0x00; // Reset Timer Values
 
 /* Insert Priority here!!! */
 IPC0bits.T1IP = 0x01; // Set Interrupt Priority (actually Level 1)
 
 IFS0bits.T1IF = 0; // Clear Interrupt Flags
 IEC0bits.T1IE = 1; // Enable Interrupts
 T1CONbits.TON = 1; // Enable the Timers    
    
    

/****** setup Timer 3 *****/
 
 
T3CONbits.TON = 0; // Disable the Timers
T3CONbits.TCKPS = 0b10; // Set Prescaler 64
T3CONbits.TCS = 0; // Set Clock Source (external = 1)
T3CONbits.TGATE = 0; // Set Gated Timer Mode -> don't use gating //this line can be ignored, if TCS =  1 (have a look at the manual)  

PR3 = 1000; // Set timer period 5ms (50 Hz):    // 1000= 5*10^-3 * 12.8*10^6 * 1/64 
TMR3 = 0x00; // Reset Timer Values
 
/* Insert Priority here!!! */
IPC2bits.T3IP = 0x02; // Set Interrupt Priority (actually Level 1)
 
IFS0bits.T3IF = 0; // Clear Interrupt Flags
IEC0bits.T3IE = 1; // Enable Interrupts
T3CONbits.TON = 1; // Enable the Timers   



/***** setup Timer 4 ******/

T4CONbits.TON = 0; // Disable the Timers
T4CONbits.TCKPS = 0b10; // Set Prescaler 64
T4CONbits.TCS = 0; // Set Clock Source (external = 1)
T4CONbits.TGATE = 0; // Set Gated Timer Mode -> don't use gating //this line can be ignored, if TCS =  1 (have a look at the manual)  

PR4 = 100; // Set timer period 0.5ms (5 Hz):    // 100= 0.5*10^-3 * 12.8*10^6 * 1/64 
TMR4 = 0x00; // Reset Timer Values
 
/* Insert Priority here!!! */
IPC6bits.T4IP = 0x03; // Set Interrupt Priority (actually Level 1)
 
IFS1bits.T4IF = 0; // Clear Interrupt Flags
IEC1bits.T4IE = 1; // Enable Interrupts
T4CONbits.TON = 1; // Enable the Timers  

}

/*
 * Servo Code
 */

 

/*
 * Touch screen code
 */



/*
 * PD Controller
 */



/*
 * Butterworth Filter N=1, Cutoff 3 Hz, sampling @ 50 Hz
 */

/*
 * ISR code 
 * 
 * Note: Timer 2 is already used for servos
 */

void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _T1Interrupt(void)
{   
    TOGGLELED(LED1_PORT);
    
    IFS0bits.T1IF = 0;      // clear interrupt service routine flag
}

void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _T3Interrupt(void)
{   
    TOGGLELED(LED2_PORT);
    
    IFS0bits.T3IF = 0;      // clear interrupt service routine flag
}

void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _T4Interrupt(void)
{   
    TOGGLELED(LED3_PORT);
    
    IFS1bits.T4IF = 0;      // clear interrupt service routine flag
}

/*
 * main loop
 */
void main_loop()
{
    // print assignment information
    lcd_printf("Lab06: Amazing Ball");
    lcd_locate(0, 1);
    lcd_printf("Group2: Boyang & Ron");
    lcd_locate(0, 2);
    
    // initialize timers
    initialize_timer();
    
    // initialize touchscreen
    initialize_touchscreen();
    
    // initialize servos
    servo_initialization(X);
    servo_initialization(Y);
    __delay_ms(2000);
   
    
    while(TRUE) {
         
        uint16_t x_value;
        uint16_t y_value;
        
        changeDimension_touchscreen(X);
        x_value = currentBallPosition();
    
        changeDimension_touchscreen(Y);
        y_value = currentBallPosition();
    
        lcd_locate(0, 5);
        lcd_printf("X/Y = %3u/%3u", x_value, y_value);
        
    }
}
