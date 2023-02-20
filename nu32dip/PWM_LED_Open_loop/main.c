#include "nu32dip.h"           // constants, funcs for startup and UART

#define NUMSAMPS 1000 // number of points in waveform
static volatile int Waveform[NUMSAMPS]; // waveform

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
    static int counter = 0; // initialize counter once
    // insert line(s) to set OC1RS
    OC1RS = Waveform[counter];
    OC1R = Waveform[counter];
    counter++; // add one to counter every time ISR is entered
    if (counter == NUMSAMPS) {
        counter = 0; // roll the counter over when needed
    }
    // insert line to clear interrupt flag
    IFS0bits.T2IF = 0;            // clear interrupt flag IFS0<3>
}

void makeWaveform() { // PWM duty cycles as a function of time
    int i = 0, center = 1200, A = 1199; // A=2399 square wave, fill in center value and amplitude
    for (i = 0; i < NUMSAMPS; ++i) {
        if ( i < NUMSAMPS/2) {
            Waveform[i] = center + A;
        } else {
            Waveform[i] = center - A;
        }
    }
}

int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  makeWaveform();

  // Assign OC1 to RA0
  RPA0Rbits.RPA0R = 0b0101; // Set A0 to OC1

  T3CONbits.TCKPS = ;      // Timer2 prescaler N=8 (1:8)
  PR3 = 2399;              // period - Correct
  TMR3 = 0;                // initial TMR2 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1RS = 0;               // duty cycle = OC1RS/(PR2+1) = X%
  OC1R = 0;                // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer2
  OC1CONbits.ON = 1;       // turn on OC1
  OC1CONbits.OCTSEL = 1;

  T2CONbits.TCKPS = 0b000; // Timer2 prescaler N=8 (1:8)
  PR2 = 47999;             // period - Correct
  TMR2 = 0;                // initial TMR2 count is 0
  T2CONbits.ON = 1;        // turn on Timer2

  __builtin_disable_interrupts(); // step 2: disable interrupts
  IPC2bits.T2IP = 5;            // step 4: interrupt priority 2
  IPC2bits.T2IS = 0;            // step 4: interrupt priority 1
  IFS0bits.T2IF = 0;            // step 5: clear the int flag
  IEC0bits.T2IE = 1;            // step 6: enable T2
  __builtin_enable_interrupts();  // step 7: enable interrupts

  while(1) { ; }  // infinite loop

  return 0;
}

