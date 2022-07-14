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

