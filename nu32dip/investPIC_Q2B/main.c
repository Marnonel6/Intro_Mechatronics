#include <stdio.h>      // input/output library
#include "nu32dip.h"          // constants, funcs for startup and UART
#include "helper.h"

/******************************************************************************
 * MAIN FUNCTION   ==SecA.4.2==
 ******************************************************************************/

int main(void) {
  
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init

  Investment inv;                // variable definition, ==SecA.4.5==

  while(getUserInput(&inv)) {    // while loop ==SecA.4.13== 
    inv.invarray[0] = inv.inv0;  // struct access ==SecA.4.4==
    calculateGrowth(&inv);       // & referencing (pointers) ==SecA.4.6, A.4.8==
    sendOutput(inv.invarray,     // passing a pointer to an array ==SecA.4.9==
               inv.years);       // passing a value, not a pointer ==SecA.4.6==
  }
  return 0;                      // return value of main ==SecA.4.6==
} // ***** END main *****
