#include "helper.h"
#include "nu32dip.h" 
#include <stdio.h> 

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

  // I/O functions in stdio.h ==SecA.4.14==
//   printf("Enter investment, growth rate, number of yrs (up to %d): ",MAX_YEARS);
  NU32DIP_WriteUART1("Enter investment, growth rate, number of yrs (up to 100):\r\n"); // send message back
  NU32DIP_ReadUART1(message, MAX_MESSAGE_LENGTH);  // get message from computer
//   scanf("%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));
  sscanf(message, "%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);
//   printf("Valid input?  %d\r\n",valid);
  char val[10];
  sprintf(val,"Valid input? %d\r\n",valid);
  NU32DIP_WriteUART1(val);

  // if-else ==SecA.4.12==
  if (!valid) { // ! is logical NOT ==SecA.4.11== 
    // printf("Invalid input; exiting.\n");
    NU32DIP_WriteUART1("Invalid input; exiting.\r\n");
  }
  return(valid);
} // ***** END getUserInput *****


void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100];      // defining a string ==SecA.4.9==

//   printf("\nRESULTS:\r\n");
  NU32DIP_WriteUART1("\r\nRESULTS:\r\n\r\n");
  for (i=0; i<=yrs; i++) {  // ++, +=, math in ==SecA.4.7==
    sprintf(outstring,"Year %3d:  %10.2f\r\n",i,arr[i]); 
    // printf("%s",outstring);
    NU32DIP_WriteUART1(outstring);
  }
//   printf("\n");
  NU32DIP_WriteUART1("\r\n");
} // ***** END sendOutput *****
