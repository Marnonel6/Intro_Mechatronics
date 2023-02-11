#ifndef CALCULATE__H__
#define CALCULATE__H__

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>

#define MAX_YEARS 100   // constant indicating max number of years to track
#define MAX_MESSAGE_LENGTH 200

typedef struct {
  double inv0;                    // initial investment
  double growth;                  // growth rate, where 1.0 = zero growth
  int years;                      // number of years to track
  double invarray[MAX_YEARS+1];   // investment array   ==SecA.4.9==
} Investment;                     // the new data type is called Investment

void calculateGrowth(Investment *invp); // ... Investment ==SecA.4.6, A.4.8==

#endif // CALCULATE__H__