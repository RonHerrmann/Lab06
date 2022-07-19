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

#define pi 3.14159265

#define X 0
#define Y 1
#define x_Axis 0
#define y_Axis 1
#define soll 350;

#define getErr(Soll, positionClean) (uint16_t)(positionClean - Soll)
#define getPR3(sampleRate) ((float) 12.8e6 * 1/sampleRate / 256)
#define getTime(counter) ((float) counter * 10 / 1000)
#define getOmega(frequency) ((float) 2 * pi * frequency)


#define radius 100
#define speed  1 // Hz
#define centerX 500
#define centerY 295


/*
 * Common Definitions
 */
#define TCKPS_1   0x00
#define TCKPS_8   0x01
#define TCKPS_64  0x02
#define TCKPS_256 0x03


/*
 * Generate Set Point
 */
   
#define generateCircleCoordinateX(center, radius, speed, counterTime) (uint16_t) (center + radius * sinf(getOmega(speed) * getTime(counterTime)))
#define generateCircleCoordinateY(center, radius, speed, counterTime) (uint16_t) (center + radius * cosf(getOmega(speed) * getTime(counterTime)))
    
   
/*
 * Butterworth Filter N=1, Cutoff 3 Hz, sampling @ 50 Hz
 */

#define b1 0.1602
#define b2 0.1602
#define a1 1
#define a2 -0.6796

#define ButterworthFilter(position, positionPrevious, positionPreviousClean) (b1 * position + b2 * positionPrevious - a2 * positionPreviousClean)
    


/*
 * Global Variables
 */

uint8_t volatile dimension = 0;
uint16_t volatile counter = 0;
uint16_t volatile iInterrupt = 0;
uint16_t volatile InterruptDeadline = 0;
uint16_t volatile deadline = 0;
uint8_t volatile flagTouch = 0;
uint8_t volatile flagServo = 0;


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
    
}


/*
 * PD Controller
 */

#define Kp 0.0845
#define Kd 0.0735

//#define setPointServoX 1.64
//#define setPointServoY 1.44

#define setPointServoX 3685
#define setPointServoY 3712

#define dutyCycle(errLocal, errPreviousLocal, setPoint) (uint16_t)((Kp * errLocal + Kd * (errLocal - errPreviousLocal)/0.02) + setPoint)

   
/*
 * ISR code 
 * 
 * Note: Timer 2 is already used for servos
 */

void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _T1Interrupt(void)   // read  the touchscreen X and Y dimension (read either X or Y and than alternate between them)
{   
    //TOGGLELED(LED1_PORT);
    
    if(InterruptDeadline == 1){  
        deadline++;  
    }
    
    flagTouch =1;   // 100 Hz
    
    if(iInterrupt == 1){  // 50 Hz
      flagServo = 1; 
      iInterrupt = 0;  
    }
    
    counter++;  // 10 ms
    
    IFS0bits.T1IF = 0;      // clear interrupt service routine flag
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
    
    uint16_t  x_positionClean = 0;
    uint16_t  y_positionClean = 0;
    uint16_t  x_position = 0;
    uint16_t  y_position = 0;
    uint16_t  x_positionPrevious = 0;
    uint16_t  y_positionPrevious = 0;
    uint16_t  x_positionPreviousClean = 0;
    uint16_t  y_positionPreviousClean = 0;
    uint16_t  setPointX = 0;
    uint16_t  setPointY = 0;
    uint16_t  dutyCycle = 0;
    
    int16_t  errX = 0;
    int16_t  errXPrevious = 0;
    int16_t  errY = 0;
    int16_t errYPrevious = 0;
    
    
   // initialize timers
    initialize_timer();
    
    // initialize touchscreen
    initialize_touchscreen();
    
    // initialize servos
    servo_initialization(X);
    servo_initialization(Y);
   //__delay_ms(500);
    
    
    while(TRUE) {
        
//        lcd_locate(0, 3);
//        lcd_printf("Counter = %3u", counter)
        
        if(flagTouch == 1){     // 100Hz
            
            InterruptDeadline = 1;
            iInterrupt ++;
            
            switch(dimension) {
                case 0: changeDimension_touchscreen(X);
                        x_positionPrevious = x_position;
                        x_position = currentBallPosition();
//                        lcd_locate(0, 3);
//                        lcd_printf("X gemessen = %3u", x_position);
                        dimension = 1; 
				break;
                case 1: changeDimension_touchscreen(Y);
                        y_positionPrevious = y_position;
                        y_position = currentBallPosition();
//                        lcd_locate(0, 4);
//                        lcd_printf("Y gemessen = %3u", y_position);
                        dimension = 0; 
				break;
                }

            InterruptDeadline = 0;
            flagTouch = 0;
        } 
        

        if(flagServo == 1){  //50 Hz
             
            setPointX = generateCircleCoordinateX(centerX, radius, speed, counter);			// generte circle coordinates
            setPointY = generateCircleCoordinateY(centerY, radius, speed, counter);
              

            x_positionClean = ButterworthFilter(x_position, x_positionPrevious, x_positionPreviousClean);
            x_positionPreviousClean = x_positionClean;
//            lcd_locate(0, 5);
//            lcd_printf("X = %3u", x_positionClean); 
            errXPrevious = errX;
//            errX = getErr(setPointX, x_positionClean); 
            errX = getErr(500, x_positionClean); 	// setPoint middle X
//            lcd_locate(0, 6);
//            lcd_printf("ErrX = %3i", errX);
//             lcd_locate(0, 6);
//             lcd_printf("X_circle = %03d", setPointX);

             
             
            dutyCycle = dutyCycle(errX, errXPrevious, setPointServoX);   // servo operation                         

            if(dutyCycle <= 3565){    //implement the allowed Range for manipulation of the servo. //2.24
                dutyCycle = 3565;
            }else if(dutyCycle >= 3805){   //1.04
                    dutyCycle = 3805;
                 }

                      
//            lcd_locate(0, 3);
//            lcd_printf("DutyCycle X = %4u", dutyCycle);
            set_dutyCycle(X, dutyCycle);
		
             

            y_positionClean = ButterworthFilter(y_position, y_positionPrevious, y_positionPreviousClean);
            y_positionPreviousClean = y_positionClean;
//            lcd_locate(0, 6);
//            lcd_printf("Y = %3u", y_positionClean);
            errYPrevious = errY;
//            errY = getErr(setPointY, y_positionClean); 
            errY = getErr(295, y_positionClean); // set Point middle y     	
//            lcd_locate(0, 7);
//            lcd_printf("ErrY = %3i", errY);
//            lcd_locate(0, 7);
//            lcd_printf("Y_circle = %03d", setPointY);


             
            dutyCycle = dutyCycle(errY, errYPrevious, setPointServoY);	 // servo operation

            if(dutyCycle <= 3592){    //implement the allowed Range for manipulation of the servo. //2.04
                dutyCycle = 3592;
            }else if(dutyCycle >= 3832){  //0.84
                dutyCycle = 3832;
            }

                       
//            lcd_locate(0, 4);
//            lcd_printf("DutyCycle Y = %4u", dutyCycle);
            set_dutyCycle(Y, dutyCycle);

            
             flagServo = 0;
         }
        
        lcd_locate(0, 3);
        lcd_printf("Deadline misses = %3u", deadline)
        
        } 
        
}


