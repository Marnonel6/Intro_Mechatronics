#ifndef UTILITIES__H__
#define UTILITIES__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "nu32dip.h"

#define MOTOR_DIRECTION LATBbits.LATB2
#define TICK_TO_DEG 3.888889// Ticks per degree (1400/360)
#define PR3_PERIOD 2400

// an enum of possible states, public in the utilities h file
enum mode_t {IDLE, PWM, ITEST, HOLD, TRACK};

// public function prototypes in the utilities h file
volatile enum mode_t get_mode(void);
void set_mode(enum mode_t s);
void setup_motor_timers_pins();


#endif // UTILITIES__H__
