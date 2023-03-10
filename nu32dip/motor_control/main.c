#include "nu32dip.h"            // config bits, constants, funcs for startup and UART
// include other header files here
#include "encoder.h"
#include "utilities.h"
#include "ina219.h"

#define BUF_SIZE 200
#define TICK_TO_DEG 3.888889// Ticks per degree (1400/360)

int main() 
{
  char buffer[BUF_SIZE];
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32DIP_GREEN = 1;  // turn off the LEDs
  NU32DIP_YELLOW = 1;        
  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  UART2_Startup();
  __builtin_enable_interrupts();

  set_mode(IDLE); //  Set PIC32 into IDLE mode

  INA219_Startup(); // Startup the current sensor

  while(1)
  {
    NU32DIP_ReadUART1(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32DIP_YELLOW = 1;                   // clear the error LED
    switch (buffer[0]) {
      case 'b': // Read current sensor (mA)
      {
        sprintf(buffer,"%f\r\n", INA219_read_current()); // add two numbers and return
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'c': // Read encoder counts
      {
        int count;
        // read encoder count
        WriteUART2("a"); // request the encoder count
        while(!get_encoder_flag()){} // wait for the Pico to respond
        set_encoder_flag(0); // clear the flag so you can read again later
        count = get_encoder_count(); // get the encoder value

        sprintf(buffer,"%d\r\n", count); // add two numbers and return
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'd': // Read encoder in degrees
      {
        int count;
        // read encoder count
        WriteUART2("a"); // request the encoder count
        while(!get_encoder_flag()){} // wait for the Pico to respond
        set_encoder_flag(0); // clear the flag so you can read again later
        count = (int)((double)get_encoder_count()/TICK_TO_DEG); // get the encoder value+

        sprintf(buffer,"%d\r\n", count); // add two numbers and return
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'e': // Reset encoder count to 0
      {
        WriteUART2("b"); // request the encoder count
        break;
      }
      case 'q':  // Quit -> Set PIC32 into IDLE mode 
      {
        set_mode(IDLE);
        break;
      }
      case 'r':  // Get the current mode of the PIC32
      {
        sprintf(buffer,"%d\r\n", get_mode()); // add two numbers and return
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'x': // add two numbers and return
      {
        int n = 0;
        int m = 0;
        NU32DIP_ReadUART1(buffer,BUF_SIZE); // Type one number then enter
        sscanf(buffer, "%d", &n);
        NU32DIP_ReadUART1(buffer,BUF_SIZE); // Type next number then enter
        sscanf(buffer, "%d", &m);
        sprintf(buffer,"%d\r\n", n + m); // add two numbers and return
        NU32DIP_WriteUART1(buffer);
        break;
      }
      default:
      {
        NU32DIP_YELLOW = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}
