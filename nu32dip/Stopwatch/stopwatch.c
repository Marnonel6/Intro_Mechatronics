#include "nu32dip.h"           // constants, funcs for startup and UART

int Flag_stopwatch = 0; // 0 - Start stopwatch , 1 - Stop stopwatch
int Flag_standby = 1;   // Wait for stopwatch to start
// int Flag_timing = 0; // Stopwatch running

void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR

  unsigned int prev_time = _CP0_GET_COUNT();
  while(_CP0_GET_COUNT() - prev_time < 24000000*0.01){;} // Wait 10 ms for bouncing to stop

  if (!PORTBbits.RB7) // Low thus pressed
  {
    if (Flag_stopwatch == 0) // Start timer
    {
        NU32DIP_WriteUART1("\r\nPress the USER button again to stop the timer!\r\n\r\n");
        _CP0_SET_COUNT(0); // Set timer to 0 thus can just read the time when user presses stop
        Flag_stopwatch = 1;
    }
    else // Stop timer
    {
        char messege[100];
        sprintf(messege, "\r\n\r\n\r\n%20.3f seconds elapsed \r\n", (float)(_CP0_GET_COUNT())/24000000.0);
        NU32DIP_WriteUART1(messege);

        if (((((float)(_CP0_GET_COUNT())/24000000.0) - 10.0) < 0.01) && ((((float)(_CP0_GET_COUNT())/24000000.0) - 10.0) > -0.01))
        {
            NU32DIP_WriteUART1("\r\n\n\n!!!!!!!!!!!!WELL DONE YOU WIN NOTHING!!!!!!!!!!!!\r\n\r\n");
        } else
        {
            NU32DIP_WriteUART1("\r\n\n\n..............You LOSE - Try again...............\r\n\r\n");
        }

        Flag_stopwatch = 0;
        Flag_standby = 1;
    }
  } // else high thus bounce on release of button. Do nothing

  IFS0bits.INT0IF = 0;            // clear interrupt flag IFS0<3>
}

int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32DIP_WriteUART1("PIC AWAKE!\r\n");
  NU32DIP_WriteUART1("\r\n\nDO YOU WANT TO PLAY A GAME?\r\n");

  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT0EP = 0;          // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 2;            // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1;            // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts
                                  // Connect RA4 (USER button) to INT0 (RB7)

  // Startup indication
  NU32DIP_GREEN = 0;  // LED1 and LED2 on
  NU32DIP_YELLOW = 0;
  _CP0_SET_COUNT(0);
  while(_CP0_GET_COUNT()<24000000*0.1){;} // Wait 100 ms
  NU32DIP_GREEN = 1;  // LED1 and LED2 off
  NU32DIP_YELLOW = 1;
  _CP0_SET_COUNT(0);
  while(_CP0_GET_COUNT()<24000000*0.1){;} // Wait 100 ms
  NU32DIP_GREEN = 0;  // LED1 and LED2 on
  NU32DIP_YELLOW = 0;
  _CP0_SET_COUNT(0);
  while(_CP0_GET_COUNT()<24000000*0.1){;} // Wait 100 ms
  NU32DIP_GREEN = 1;  // LED1 and LED2 off
  NU32DIP_YELLOW = 1;

  while(1) {
      if (Flag_standby)
      {
        unsigned int prev_time = _CP0_GET_COUNT();
        while(_CP0_GET_COUNT() - prev_time < (24000000*3)){;} // Wait 3 seunsigned int prev_time = _CP0_GET_COUNT();conds
        NU32DIP_WriteUART1("\r\n\n\n\nPress the USER button to start the timer!\r\n");
        NU32DIP_WriteUART1("\r\n\nStop the timer at 10 seconds to WIN the JACKPOT!!!!\r\n");
        Flag_standby = 0;
      }
      else if (Flag_stopwatch)
      {
        NU32DIP_GREEN = 0;  // LED1 and LED2 on
        NU32DIP_YELLOW = 0;
        unsigned int prev_time = _CP0_GET_COUNT();
        while(_CP0_GET_COUNT() - prev_time < (24000000*0.01)*2){;} // Only print every 20ms
        char messege[100];
        if (Flag_stopwatch) // Stops (most of the time) a print of this after the timer was stoped
        {
            sprintf(messege, "%20.3f running time\r", (float)(_CP0_GET_COUNT())/24000000.0);
            NU32DIP_WriteUART1(messege);
        }
        NU32DIP_GREEN = 1;  // LED1 and LED2 off
        NU32DIP_YELLOW = 1;
      }
  }

  return 0;
}
