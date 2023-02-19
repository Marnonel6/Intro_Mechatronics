#include "nu32dip.h"           // constants, funcs for startup and UART

int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init

  // Assign OC1 to RA0
  RPA0Rbits.RPA0R = 0b0101; // Set A0 to OC1

  T2CONbits.TCKPS = 0;     // Timer2 prescaler N=1 (1:1)
  PR2 = 2399;              // period
  TMR2 = 0;                // initial TMR2 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1RS = 1800;            // duty cycle = OC1RS/(PR2+1) = 75%
  OC1R = 0;                // initialize before turning OC1 on; afterward it is read-only
  T2CONbits.ON = 1;        // turn on Timer2
  OC1CONbits.ON = 1;       // turn on OC1

  while(1) { ; }  // infinite loop

  return 0;
}

