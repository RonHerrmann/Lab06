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
#define soll 350;

#define getErr(Soll, positionClean) (Soll - positionClean)
#define getPR3(sampleRate) ((float) 12.8e6 * 1/sampleRate / 256)

#define radius 100
#define speed  50 // Hz
#define centerX 350
#define centerY 350


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

uint8_t volatile dimension = 0;
uint16_t volatile iInterrupt = 0;
uint16_t volatile iInterrupt1 = 1;
uint8_t volatile flagTouch = 0;
uint8_t volatile flagServo = 0;
uint8_t volatile flagCircle = 0;

//uint16_t volatile x_positionClean;
//uint16_t volatile y_positionClean;
//uint16_t volatile x_position = 0;
//uint16_t volatile y_position = 0;
//uint16_t volatile x_positionPrevious = 0;
//uint16_t volatile y_positionPrevious = 0;
//uint16_t volatile x_positionPreviousClean = 0;
//uint16_t volatile y_positionPreviousClean = 0;

//int16_t volatile errX = 0;
//int16_t volatile errXPrevious = 0;
//int16_t volatile errY = 0;
//int16_t volatile errYPrevious = 0;


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
 * Servo Code
 */

 

/*
 * Touch screen code
 */



/*
 * PD Controller
 */

#define Kp 0.001
#define Kd 0 //0.000005


void PD_Conctroller_Axis(int16_t err, int16_t errPrevious, uint8_t servo){
    
    float dutyCycle = 0;
    
    if(servo == X){
      dutyCycle = (float)((Kp * err + Kd * (err - errPrevious)/0.02) + 1.5);
      
            if (dutyCycle >= 2.1){    //implement the allowed Range for manipulation of the servo.
                dutyCycle = 2.1;
            }else if(dutyCycle<= 0.9){
                dutyCycle = 0.9;
            }
    }
    else if(servo == Y){
            dutyCycle = (float)((Kp * err + Kd * (err - errPrevious)/0.02) + 1.5);
            
            if (dutyCycle >= 2.1){    //implement the allowed Range for manipulation of the servo.
                dutyCycle = 2.1;
            }else if(dutyCycle<= 0.9){
                dutyCycle = 0.9;
            }
    }
    
   
    lcd_locate(0, 5);
    lcd_printf("DutyCycle = %3.2f", dutyCycle);
    
    set_dutyCycle(servo, dutyCycle);
    
}


//void generateCircle(){
//    
////    setPointX = centerX - radius;
////    setPointY = 
//    
//}



/*
 * Butterworth Filter N=1, Cutoff 3 Hz, sampling @ 50 Hz
 */

#define b1 0.1602
#define b2 0.1602
#define a1 1
#define a2 -0.6796

uint16_t ButterworthFilter(uint16_t position, uint16_t positionPrevious, uint16_t positionPreviousClean){
    
    uint16_t positionClean;
     
    positionClean = b1 * position + b2 * positionPrevious - a2 * positionPreviousClean;
    
    return positionClean;
}


/*
 * ISR code 
 * 
 * Note: Timer 2 is already used for servos
 */

void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _T1Interrupt(void)   // read  the touchscreen X and Y dimension (read either X or Y and than alternate between them)
{   
    //TOGGLELED(LED1_PORT);
    
    flagTouch =1;   // 100 Hz
    
    if(iInterrupt == 1){  // 50 Hz
      flagServo = 1; 
      iInterrupt = 0;  
    }
    
//    if(iInterrupt1 == 4){  // speed for circle Hz 1/speed/0.01
//      flagCircle = 1; 
//      iInterrupt1 = 0;  
//    }
//    
//    
//    iInterrupt1 ++;
    
    
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
    
    uint16_t  x_positionClean;
    uint16_t  y_positionClean;
    uint16_t  x_position = 0;
    uint16_t  y_position = 0;
    uint16_t  x_positionPrevious = 0;
    uint16_t  y_positionPrevious = 0;
    uint16_t  x_positionPreviousClean = 0;
    uint16_t  y_positionPreviousClean = 0;
    
    int16_t  errX = 0;
    int16_t  errXPrevious = 0;
    int16_t  errY = 0;
    int16_t  errYPrevious = 0;
    
    
   // initialize timers
    initialize_timer();
    
    // initialize touchscreen
    initialize_touchscreen();
    
    // initialize servos
    servo_initialization(X);
    servo_initialization(Y);
    __delay_ms(1000);
    
    
    while(TRUE) {
        
        if(flagTouch == 1){     // 100Hz
            
            iInterrupt ++;
            
            switch(dimension) {
                case 0: changeDimension_touchscreen(X);
                        x_positionPrevious = x_position;
                        x_position = currentBallPosition();
//                        lcd_locate(0, 3);
//                        lcd_printf("X gemessen = %3u", x_position)
                        x_positionClean = ButterworthFilter(x_position, x_positionPrevious, x_positionPreviousClean);
                        x_positionPreviousClean = x_positionClean;
//                        lcd_locate(0, 5);
//                        lcd_printf("X = %3u", x_positionClean); 
                        dimension = 1; break;
                case 1: changeDimension_touchscreen(Y);
                        y_positionPrevious = y_position;
                        y_position = currentBallPosition();
//                        lcd_locate(0, 4);
//                        lcd_printf("Y gemessen = %3u", y_position)
                        y_positionClean = ButterworthFilter(y_position, y_positionPrevious, y_positionPreviousClean);
                        y_positionPreviousClean = y_positionClean;
//                        lcd_locate(0, 6);
//                        lcd_printf("Y = %3u", y_positionClean);
                        dimension = 0; break;

                }
            flagTouch = 0;
        } 
        
         if(flagServo == 1){  //50 Hz
             
             errX = getErr(350, x_positionClean);
             lcd_locate(0, 6);
             lcd_printf("ErrX = %03d", errX);
             
             PD_Conctroller_Axis(errX, errXPrevious, X);
             
             errY = getErr(350, y_positionClean);
             lcd_locate(0, 7);
             lcd_printf("ErrY = %03d", errY);
             
             PD_Conctroller_Axis(errY, errYPrevious, Y);

             
            flagServo = 0;
         }
        
        if(flagCircle == 1){  // Speed
             
             

             
            flagCircle = 0;
         }
        
        
        } 
        
}

