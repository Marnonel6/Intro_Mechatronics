#ifndef ULTRA__H__
#define ULTRA__H__

#define MAX_MESSAGE_LENGHT 200
#define TRIG LATBbits.LATB6
#define ECHO LATBbits.LATB8
#define SOUND_SPEED 340 // [m/s]
#define MAX_DISTANCE 4 // [m]

void init_ultra();
unsigned int read_ultra();

#endif // ULTRA__H__
