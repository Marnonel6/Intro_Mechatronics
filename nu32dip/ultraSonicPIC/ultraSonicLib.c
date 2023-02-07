#include "nu32dip.h"
#include "ultraSonicLib.h"

void init_ultra()
{
    // b6 as output to trig pin of UltraSonic Sensor - Init off
    // b7 as input to echo pin of UltraSonic Sensor
    TRISBbits.TRISB7 = 1;
    TRISBbits.TRISB6 = 0;
    TRIG = 0;
}

unsigned int read_ultra()
{
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

    return _CP0_GET_COUNT(); // Get time waited for ultra sonic wave to return to sensor
}