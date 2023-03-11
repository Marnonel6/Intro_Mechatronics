#include "nu32dip.h"            // config bits, constants, funcs for startup and UART
// include other header files here
#include "encoder.h"
#include "utilities.h"
#include "ina219.h"

#define BUF_SIZE 200
#define INTEGRAL_LIMIT 400.0    // Limit integral windup
#define INTEGRAL_LIMIT_POS 30.0 // Limit integral windup
#define ITEST_PLOTPTS 100       // number of data points to plot
#define POS_CONTROLLER_DT 0.005 // Change in time for derivative velocity calculation for positional control
#define MAX_CURRENT 70.0        // Current at 100% PWM of motor

static volatile int set_pwm = 0;                    // User defined PWM from menu option f
static volatile float set_angle = 0.0;              // Desired angle by user
// Current PI controller
static volatile float Kp = 0.015;                     // Proportional gain
static volatile float Ki = 0.15;                     // Integral gain
static volatile int Desired_Current[ITEST_PLOTPTS]; // measured values to plot
static volatile int Actual_Current[ITEST_PLOTPTS];  // reference values to plot
// Position PID controller
static volatile float Kp_pos = 0.0;                 // Proportional gain
static volatile float Kd_pos = 0.0;                 // Derivative gain
static volatile float Ki_pos = 0.0;                 // Integral gain
static volatile float desired_amp_pos = 0.0;        // Position control desired amps
static volatile int Desired_Position[200]; // measured values to plot
static volatile int Actual_Position[200];  // reference values to plot

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Current_Controller(void) { // 5kHz control for motor current control

    switch(get_mode()){
        case IDLE:
        {
            // set pwm duty to 0 and direction to 0
            OC1RS = 0;
            MOTOR_DIRECTION = 0; // Clockwise
            break;
        }
        case PWM:
        {
            // Set PWM and direction based on user input from menu f
            if (set_pwm<0)
            {
              MOTOR_DIRECTION = 1; // Anti-Clockwise
              OC1RS = (int)(PR3_PERIOD*((float)set_pwm/-100.0));
            }
            else if (set_pwm>=0)
            {
              MOTOR_DIRECTION = 0; // Clockwise
              OC1RS = (int)(PR3_PERIOD*((float)set_pwm/100.0));
            }
            break;
        }
        case ITEST:
        {
            // Set static variables
            static int square_count = 0;
            static float desired_amp = 0.0;
            static float integral_error = 0.0;
            static int plotind = 0; // index for data arrays; counts up to PLOTPTS

            if (square_count < 25 || (square_count >= 50 && square_count < 75))
            {
              desired_amp = 30.0; // [mA]
            }
            else if ((square_count >= 25 && square_count < 50) || (square_count >= 75 && square_count < 99))
            {
              desired_amp = -30.0; // [mA]
            }
            else if (square_count >= 99)
            {
              square_count = 0; // Reset counter for next test
              set_mode(IDLE);
              integral_error = 0.0;
              desired_amp = 0.0;
              plotind = 0;
            }

            // PI Control
            float error = desired_amp - INA219_read_current(); // Calculate error
            integral_error += error;
            // Limit integral windup
            if (integral_error > INTEGRAL_LIMIT){
                integral_error = INTEGRAL_LIMIT;
            } else if (integral_error < -INTEGRAL_LIMIT)
            {
                integral_error = -INTEGRAL_LIMIT;
            }
            float u_current = Kp*error + Ki*integral_error;
            if (u_current > 100.0) { // 100% duty cycle
                u_current = 100.0;
            } else if (u_current < -100.0) {
                u_current = -100.0;
            }

            // Set PWM and direction based on user input from menu f
            if (u_current<0)
            {
              MOTOR_DIRECTION = 0; // Anti-Clockwise
              OC1RS = (int)(PR3_PERIOD*((float)u_current/-100.0));
            }
            else if (u_current>=0)
            {
              MOTOR_DIRECTION = 1; // Clockwise
              OC1RS = (int)(PR3_PERIOD*((float)u_current/100.0));
            }
            square_count++;

            Desired_Current[plotind] = (int)desired_amp; // store data in global arrays
            Actual_Current[plotind] = (int)INA219_read_current();
            plotind++; // increment plot data index

            if (plotind == ITEST_PLOTPTS)
            { // if max number of plot points plot is reached,
                plotind = 0; // reset the plot index
            }

            break;
        }
        case HOLD:
        {
            //
            // Set static variables
            static int square_count = 0;
            static float integral_error = 0.0;
            static int plotind = 0; // index for data arrays; counts up to PLOTPTS

            // else if (square_count >= 99)
            // {
            //   square_count = 0; // Reset counter for next test
            //   set_mode(IDLE);
            //   integral_error = 0.0;
            //   desired_amp = 0.0;
            //   plotind = 0;
            // }

            // PI Control
            float error = desired_amp_pos - INA219_read_current(); // Calculate error
            integral_error += error;
            // Limit integral windup
            if (integral_error > INTEGRAL_LIMIT){
                integral_error = INTEGRAL_LIMIT;
            } else if (integral_error < -INTEGRAL_LIMIT)
            {
                integral_error = -INTEGRAL_LIMIT;
            }
            float u_current = Kp*error + Ki*integral_error;
            if (u_current > 100.0) { // 100% duty cycle
                u_current = 100.0;
            } else if (u_current < -100.0) {
                u_current = -100.0;
            }

            // Set PWM and direction based on user input from menu f
            if (u_current<0)
            {
              MOTOR_DIRECTION = 0; // Anti-Clockwise
              OC1RS = (int)(PR3_PERIOD*((float)u_current/-100.0));
            }
            else if (u_current>=0)
            {
              MOTOR_DIRECTION = 1; // Clockwise
              OC1RS = (int)(PR3_PERIOD*((float)u_current/100.0));
            }
            // square_count++;

            // Desired_Current[plotind] = (int)desired_amp; // store data in global arrays
            // Actual_Current[plotind] = (int)INA219_read_current();
            // plotind++; // increment plot data index

            // if (plotind == ITEST_PLOTPTS)
            // { // if max number of plot points plot is reached,
            //     plotind = 0; // reset the plot index
            // }

            break;
        }
        case TRACK:
        {
            //
            break;
        }
    }

    IFS0bits.T2IF = 0;            // clear interrupt flag IFS0<3>
}

void __ISR(_TIMER_4_VECTOR, IPL5SOFT) Position_Controller(void) { // 200Hz control loop for motor position control

    switch(get_mode()){
        case HOLD:
        {
            //

            // Set static variables
            // static int square_count = 0;
            // static float desired_amp = 0.0;
            static float integral_error_pos = 0.0;
            static int plotind = 0; // index for data arrays; counts up to PLOTPTS
            static float prev_error = 0;

            // else if (square_count >= 99)
            // {
            //   square_count = 0; // Reset counter for next test
            //   set_mode(IDLE);
            //   integral_error = 0.0;
            //   desired_amp = 0.0;
            //   plotind = 0;
            // }

            // read encoder count
            WriteUART2("a"); // request the encoder count
            while(!get_encoder_flag()){} // wait for the Pico to respond
            set_encoder_flag(0); // clear the flag so you can read again later

            // PI Control
            float error_pos = -(set_angle - (float)(get_encoder_count()/TICK_TO_DEG)); // Calculate Angle error
            float error_velocity = (error_pos - prev_error)/POS_CONTROLLER_DT;
            prev_error = error_pos;
            integral_error_pos += error_pos;

            // Limit integral windup
            if (integral_error_pos > INTEGRAL_LIMIT_POS){
                integral_error_pos = INTEGRAL_LIMIT_POS;
            } else if (integral_error_pos < -INTEGRAL_LIMIT_POS)
            {
                integral_error_pos = -INTEGRAL_LIMIT_POS;
            }

            float desired_current = Kp_pos*error_pos + Kd_pos*error_velocity + Ki_pos*integral_error_pos;
            if (desired_current > MAX_CURRENT) { // 100% duty cycle
                desired_current = MAX_CURRENT;
            } else if (desired_current < -MAX_CURRENT) {
                desired_current = -MAX_CURRENT;
            }

            desired_amp_pos = desired_current;

            Desired_Position[plotind] = set_angle;// (int)desired_amp_pos; // store data in global arrays
            Actual_Position[plotind] = (int)(get_encoder_count()/TICK_TO_DEG);
            plotind++; // increment plot data index

            if (plotind == 200)
            { // if max number of plot points plot is reached,
                plotind = 0; // reset the plot index
                set_mode(IDLE);
                prev_error = 0;
            }

            break;
        }
        case TRACK:
        {
            //
            break;
        }
    }

    IFS0bits.T4IF = 0;            // clear interrupt flag IFS0<3>
}

int main() 
{
  char buffer[BUF_SIZE];
  NU32DIP_Startup();  // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32DIP_GREEN = 1;  // turn off the LEDs
  NU32DIP_YELLOW = 1;

  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  UART2_Startup();
  setup_motor_timers_pins();                // Setup timers and interrupts for motor control
  setup_position_controller_timer();         //  Setup positional controller timer interrupt
  set_mode(IDLE);                           // Set PIC32 into IDLE mode
  INA219_Startup();                         // Startup the current sensor

  __builtin_enable_interrupts();

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
        // count = (int)((double)get_encoder_count()/TICK_TO_DEG); // get the encoder value+
        count = (int)(get_encoder_count()/TICK_TO_DEG);

        sprintf(buffer,"%d\r\n", count); // add two numbers and return
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'e': // Reset encoder count to 0
      {
        WriteUART2("b"); // request the encoder count
        break;
      }
      case 'f': // Set PWM (-100 to 100) and direction
      {
        NU32DIP_ReadUART1(buffer,BUF_SIZE); // Type pwm number then enter
        sscanf(buffer, "%d", &set_pwm);
        set_mode(PWM);

        // Limit PWM to +-100
        if (set_pwm<-100){
            set_pwm = -100;
        }
        else if (set_pwm>100){
            set_pwm = 100;
        }

        break;
      }
      case 'g': // Set current gains
      {
        NU32DIP_ReadUART1(buffer,BUF_SIZE); // Type one number then enter
        sscanf(buffer, "%f", &Kp);
        NU32DIP_ReadUART1(buffer,BUF_SIZE); // Type next number then enter
        sscanf(buffer, "%f", &Ki);
        break;
      }
      case 'h': // Get current gains
      {
        sprintf(buffer,"%10.5f\r\n", Kp);
        NU32DIP_WriteUART1(buffer);
        sprintf(buffer,"%10.5f\r\n", Ki);
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'i': // Set position gains
      {
        NU32DIP_ReadUART1(buffer,BUF_SIZE); // Type one number then enter
        sscanf(buffer, "%f", &Kp_pos);
        NU32DIP_ReadUART1(buffer,BUF_SIZE); // Type next number then enter
        sscanf(buffer, "%f", &Kd_pos);
        NU32DIP_ReadUART1(buffer,BUF_SIZE); // Type next number then enter
        sscanf(buffer, "%f", &Ki_pos);
        break;
      }
      case 'j': // Get position gains
      {
        sprintf(buffer,"%10.5f\r\n", Kp_pos);
        NU32DIP_WriteUART1(buffer);
        sprintf(buffer,"%10.5f\r\n", Kd_pos);
        NU32DIP_WriteUART1(buffer);
        sprintf(buffer,"%10.5f\r\n", Ki_pos);
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'k': // Test current controller
      {
        sprintf(buffer,"%d\r\n", ITEST_PLOTPTS);
        NU32DIP_WriteUART1(buffer);
        set_mode(ITEST); // Set mode to ITEST
        while (get_mode() == ITEST){;}
        for (int i = 0; i < ITEST_PLOTPTS; i++)
        {
            // Send desired and actual currents
            sprintf(buffer,"%d %d\r\n", Desired_Current[i], Actual_Current[i] );
            NU32DIP_WriteUART1(buffer);
        }
        break;
      }
      case 'l': // Go to angle (deg)
      {
        NU32DIP_ReadUART1(buffer,BUF_SIZE); // Type one number then enter
        sscanf(buffer, "%f", &set_angle);

        WriteUART2("b"); // RESET the encoder count

        sprintf(buffer,"%d\r\n", 200);
        NU32DIP_WriteUART1(buffer);
        set_mode(HOLD); // Set mode to ITEST
        while (get_mode() == HOLD){;}
        for (int i = 0; i < 200; i++)
        {
            // Send desired and actual currents
            sprintf(buffer,"%d %d\r\n", Desired_Position[i], Actual_Position[i] );
            NU32DIP_WriteUART1(buffer);
        }
        break;
      }
      case 'p': // Unpower the motor
      {
        set_mode(IDLE); // Set mode to IDLE
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
        NU32DIP_YELLOW = 0;  // turn on YELLOW to indicate an error
        break;
      }
    }
  }
  return 0;
}