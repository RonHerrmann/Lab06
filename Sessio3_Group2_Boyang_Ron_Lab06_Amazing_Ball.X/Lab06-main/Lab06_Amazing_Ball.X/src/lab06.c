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
 * main loop
 */
void main_loop()
{
    // print assignment information
    lcd_printf("Lab06: Amazing Ball");
    lcd_locate(0, 1);
    lcd_printf("Group2: Boyang & Ron");
    lcd_locate(0, 2);
    
    
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
