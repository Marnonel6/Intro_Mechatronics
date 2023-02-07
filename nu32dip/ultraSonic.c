#include "nu32dip.h"
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

#define MAX_MESSAGE_LENGHT 200
#define TRIG LATBbits.LATB6
#define ECHO LATBbits.LAT7
#define SOUND_SPEED 340 // [m/s]
#define MAX_DISTANCE 4 // [m]

int main(void){
    char messege[MAX_MESSAGE_LENGHT];

    NU32DIP_Startup();
    NU32DIP_WriteUART1("PIC AWAKE!\r\n");

    // b6 as output to trig pin of UltraSonic Sensor - Init off
    // b7 as input to echo pin of UltraSonic Sensor
    TRISBbits.TRISB7 = 1;
    TRISBbits.TRISB6 = 0;
    TRIG = 0;

    while(1){

        // Trigger ultra sonic wave output
        _CP0_SET_COUNT(0);
        TRIG = 1; // Set TRIG pin to high
        while(_CP0_GET_COUNT()<24000000*0.00001){} // Wait 10us
        TRIG = 0; // Set TRIG pin to low

        // Wait for echo pin to go high
        while(PORTBbits.RB7 == 0){} // While pin low wait for echo pin to start listening to wave
        _CP0_SET_COUNT(0);
        // Wait for echo pin to go low
        while(PORTBbits.RB7 == 1){} // While pin high wait for ultra sonic wave to be sensed
        unsigned int time = _CP0_GET_COUNT(); // Get time waited for ultra sonic wave to return to sensor
        // Calculate distance
        float distance = ((float)time/2.0)/24000000*SOUND_SPEED;
        if (distance<MAX_DISTANCE)
        {
            sprintf(messege, "%f\r\n", distance);
            NU32DIP_WriteUART1(messege);
        }
    }
    return 0;
}