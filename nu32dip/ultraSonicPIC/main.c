#include "ultraSonicLib.h"
#include "nu32dip.h"

int main(void){
    char messege[MAX_MESSAGE_LENGHT];

    NU32DIP_Startup();
    NU32DIP_WriteUART1("PIC AWAKE!\r\n");
    init_ultra();

    while(1){
        unsigned int time = read_ultra();
        // Calculate distance
        float distance = ((float)time/2.0)/24000000*SOUND_SPEED;
        if (distance<MAX_DISTANCE)
        {
            sprintf(messege, "Distance: %2.3f [m] \r\n", distance);
            NU32DIP_WriteUART1(messege);
        }
    }
    return 0;
}