#include <stdio.h>      // input/output library
#include "nu32dip.h"          // constants, funcs for startup and UART
#define MAX_YEARS 100   // constant indicating max number of years to track
#define MAX_MESSAGE_LENGTH 200

typedef struct {
  double inv0;                    // initial investment
  double growth;                  // growth rate, where 1.0 = zero growth
  int years;                      // number of years to track
  double invarray[MAX_YEARS+1];   // investment array   ==SecA.4.9==
} Investment;                     // the new data type is called Investment

int getUserInput(Investment *invp);     // invp is a pointer to type ...
void calculateGrowth(Investment *invp); // ... Investment ==SecA.4.6, A.4.8==
void sendOutput(double *arr, int years);

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


void calculateGrowth(Investment *invp) {

  int i;

  // for loop ==SecA.4.13==
  for (i = 1; i <= invp->years; i= i + 1) {   // relational operators ==SecA.4.10==
                                              // struct access ==SecA.4.4==
    invp->invarray[i] = invp->growth * invp->invarray[i-1]; 
  }
} // ***** END calculateGrowth *****


int getUserInput(Investment *invp) {

  int valid;       // int used as a boolean ==SecA.4.10==
  char message[MAX_MESSAGE_LENGTH];

// Changed Code
//   printf("Enter investment, growth rate, number of yrs (up to %d): ",MAX_YEARS);
  NU32DIP_WriteUART1("Enter investment, growth rate, number of yrs (up to 100):\r\n"); // send message back
  NU32DIP_ReadUART1(message, MAX_MESSAGE_LENGTH);  // get message from computer
//   scanf("%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));
  sscanf(message, "%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);
//   printf("Valid input?  %d\r\n",valid);
  char val[10];
  // Changed Code
  sprintf(val,"Valid input? %d\r\n",valid);
  NU32DIP_WriteUART1(val);

  if (!valid) { // ! is logical NOT ==SecA.4.11== 
    // printf("Invalid input; exiting.\n");
    // Changed Code
    NU32DIP_WriteUART1("Invalid input; exiting.\r\n");
  }
  return(valid);
} // ***** END getUserInput *****


void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100];      // defining a string ==SecA.4.9==

//   printf("\nRESULTS:\r\n");
  // Changed Code
  NU32DIP_WriteUART1("\r\nRESULTS:\r\n\r\n");
  for (i=0; i<=yrs; i++) {  // ++, +=, math in ==SecA.4.7==
    // Changed Code
    sprintf(outstring,"Year %3d:  %10.2f\r\n",i,arr[i]); 
    // printf("%s",outstring);
    // Changed Code
    NU32DIP_WriteUART1(outstring);
  }
//   printf("\n");
  // Changed Code
  NU32DIP_WriteUART1("\r\n");
} // ***** END sendOutput *****
