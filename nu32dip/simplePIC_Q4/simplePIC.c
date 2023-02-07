#include <xc.h>          // Load the proper header for the processor

void delay(void);

int main(void) {
  TRISBCLR = 0x30; // Make TRIS for B4 and B5 0, 
  LATBbits.LATB4 = 0;    // Turn GREEN on and YELLOW off.  These pins sink current
  LATBbits.LATB5 = 0;   // This is for Ch3 Question 4

  while(1) {
    delay();
    LATBINV = 0b110000;    // toggle GREEN and YELLOW;
  }
  return 0;
}

void delay(void) {
  int j;
  for (j = 0; j < 1000000; j++) { // number is 1 million
    while(!PORTAbits.RA4) {
        ;   // Pin A4 is the USER switch, low (FALSE) if pressed.
    }
    }
  }
