#include <xc.h>          // Load the proper header for the processor

// #define MAXCYCLES 1000000; // This is for Ch3 Question 5
// #define DELTACYCLES 100000; // This is for Ch3 Question 5

void delay(void);
// void delay(int cycles); // This is for Ch3 Question 5

// int cycles; // This is for Ch3 Question 5

int main(void) {
  TRISBCLR = 0x30; // Make TRIS for B4 and B5 0, 
  LATBbits.LATB4 = 0;    // Turn GREEN on and YELLOW off.  These pins sink current
  LATBbits.LATB5 = 1;    // on the NU32DIP, so "high" (1) = "off" and "low" (0) = "on"
//   LATBbits.LATB5 = 0;   // This is for Ch3 Question 4

//   cycles = MAXCYCLES; // This is for Ch3 Question 5

  while(1) {
    delay(); // This is for Ch3 Question 5
    // delay(cycles); // This is for Ch3 Question 5
    LATBINV = 0b110000;    // toggle GREEN and YELLOW;
    // cycles -= DELTACYCLES; // This is for Ch3 Question 5
    // if (cycles<=0) // This is for Ch3 Question 5
    // {
    //     cycles = MAXCYCLES;
    // }
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

// // This is for Ch3 Question 5
// void delay(int cycles) {
//   int j;
//   for (j = 0; j < cycles; j++) { // number is 1 million
//     while(!PORTAbits.RA4) {
//         ;   // Pin A4 is the USER switch, low (FALSE) if pressed.
//     }
//     }
//   }

