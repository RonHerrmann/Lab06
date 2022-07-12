#include "lab06_servos.h"

#include <xc.h>
//do not change the order of the following 2 definitions
#define FCY 12800000UL
#include <libpic30.h>

#include "types.h"
#include "lcd.h"

#define X 0
#define Y 1


/*
 * Function to initilize servo
 * 
 * input: servo x or y
 */

void servo_initialization(uint8_t servo)
{
    //setup timer 2
    CLEARBIT(T2CONbits.TON); // Disable Timer
    CLEARBIT(T2CONbits.TCS); // Select internal instruction cycle clock
    CLEARBIT(T2CONbits.TGATE); // Disable Gated Timer mode
	
    TMR2 = 0x00; // Clear timer register
    T2CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
	
    CLEARBIT(IFS0bits.T2IF); // Clear Timer2 interrupt status flag
    CLEARBIT(IEC0bits.T2IE); // Disable Timer2 interrupt enable control bit
	
    PR2 = 4000; // Set timer period 20ms:    // 4000= 20*10^-3 * 12.8*10^6 * 1/64
	
    if(servo == X){
		
        //setup OC8
		
        CLEARBIT(TRISDbits.TRISD7); /* Set OC8 as output */
        OC8R = 3700; 
        OC8RS = 3700; 
        OC8CON = 0x0006; /* Set OC8: PWM, no fault check, Timer2 because the 4th bit is 0(1for timer3)*/
        
    }else if(servo == Y){
		
        //setup OC7
		
        CLEARBIT(TRISDbits.TRISD6); /* Set OC7 as output, */
        OC7R = 3700; 
        OC7RS = 3700; 
        OC7CON = 0x0006; /* Set OC8: PWM, no fault check, Timer2 because the 4th bit is 0(1for timer3)*/
    }
    
    SETBIT(T2CONbits.TON); /* Turn Timer 2 on */
}


/*
 * Function to set servo to needed PWM signal
 * 
 * input: servo x or y, needed duty cycle of PMW signal
 */

void set_dutyCycle(uint8_t servo, float dutyCycle){

    if(servo == X){
        //setup OC8
        OC8RS = (uint16_t)(4000-dutyCycle*200); /* Load OCRS: next pwm duty cycle , 4000-XX because of inversion*/
          
    }else if(servo == Y){
        //setup OC7
        OC7RS = (uint16_t)(4000-dutyCycle*200); /* Load OCRS: next pwm duty cycle */
    }

}

