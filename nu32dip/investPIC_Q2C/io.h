#ifndef IO__H__
#define IO__H__

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>
#include "calculate.h"

int getUserInput(Investment *invp);     // invp is a pointer to type ...
void sendOutput(double *arr, int years);

#endif // IO__H__