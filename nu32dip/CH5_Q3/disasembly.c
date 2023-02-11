#include "nu32dip.h"          // constants, funcs for startup and UART

int main(void) {
    char c1=5, c2=6, c3;
    int i1=5, i2=6, i3;
    long long int j1=5, j2=6, j3;
    float f1=1.01, f2=2.02, f3;
    long double d1=1.01, d2=2.02, d3;
  
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init

  while (1) {
    c3 = c1+c2;
    c3 = c1-c2;
    c3 = c1*c2;
    c3 = c1/c2;
  }
  return 0;
}
