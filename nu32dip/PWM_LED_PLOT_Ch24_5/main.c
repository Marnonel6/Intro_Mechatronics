#include "nu32dip.h"           // constants, funcs for startup and UART

#define NUMSAMPS 1000 // number of points in waveform
#define NUMSAMPS 1000 // number of points in waveform
#define PLOTPTS 200 // number of data points to plot
#define DECIMATION 10 // plot every 10th point

static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS]; // measured values to plot
static volatile int REFarray[PLOTPTS]; // reference values to plot
static volatile int StoringData = 0; // if this flag = 1, currently storing
// plot data
static volatile float Kp = 0, Ki = 0; // control gains

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {

    static int counter = 0; // initialize counter once
    static int plotind = 0; // index for data arrays; counts up to PLOTPTS
    static int decctr = 0; // counts to store data one every DECIMATION
    static int adcval = 0; //
    // insert line(s) to set OC1RS
    OC1RS = Waveform[counter];

    if (StoringData) {
        decctr++;
        if (decctr == DECIMATION) { // after DECIMATION control loops,
            decctr = 0; // reset decimation counter
            ADCarray[plotind] = adcval; // store data in global arrays
            REFarray[plotind] = Waveform[counter];
            plotind++; // increment plot data index
        }
        
        if (plotind == PLOTPTS) { // if max number of plot points plot is reached,
            plotind = 0; // reset the plot index
            StoringData = 0; // tell main data is ready to be sent to MATLAB
        }
    }
    
    counter++; // add one to counter every time ISR is entered
    
    if (counter == NUMSAMPS) {
        counter = 0; // rollover counter over when end of waveform reached
    }

    // insert line to clear interrupt flag
    IFS0bits.T2IF = 0;            // clear interrupt flag IFS0<3>
}

void makeWaveform() { // PWM duty cycles as a function of time
    int i = 0, center = 1200, A = 1199; // square wave, fill in center value and amplitude
    for (i = 0; i < NUMSAMPS; ++i) {
        if ( i < NUMSAMPS/2) {
            Waveform[i] = center + A;
        } else {
            Waveform[i] = center - A;
        }
    }
}

int main(void) {
    NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    makeWaveform();

    char message[100]; // message to and from MATLAB
    float kptemp = 0, kitemp = 0; // temporary local gains
    int i = 0; // plot data loop counter

    // Assign OC1 to RA0
    RPA0Rbits.RPA0R = 0b0101; // Set A0 to OC1

    T3CONbits.TCKPS = 0b001; // Timer3 prescaler
    PR3 = 2399;              // period - Correct
    TMR3 = 0;                // initial TMR3 count is 0
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC1RS = 0;               // duty cycle = OC1RS/(PR3+1) = X%
    OC1R = 0;                // initialize before turning OC1 on; afterward it is read-only
    T3CONbits.ON = 1;        // turn on Timer3
    OC1CONbits.ON = 1;       // turn on OC1
    OC1CONbits.OCTSEL = 1;

    T2CONbits.TCKPS = 0b000; // Timer2 prescaler
    PR2 = 47999;             // period - Correct
    TMR2 = 0;                // initial TMR2 count is 0
    T2CONbits.ON = 1;        // turn on Timer2

    __builtin_disable_interrupts(); // step 2: disable interrupts
    IPC2bits.T2IP = 5;            // step 4: interrupt priority 2
    IPC2bits.T2IS = 0;            // step 4: interrupt priority 1
    IFS0bits.T2IF = 0;            // step 5: clear the int flag
    IEC0bits.T2IE = 1;            // step 6: enable T2
    __builtin_enable_interrupts();  // step 7: enable interrupts

    while(1) {
        NU32DIP_ReadUART1(message, sizeof(message)); // wait for a message from MATLAB
        sscanf(message, "%f %f" , &kptemp, &kitemp);

        __builtin_disable_interrupts(); // keep ISR disabled as briefly as possible
        Kp = kptemp; // copy local variables to globals used by ISR
        Ki = kitemp;
        __builtin_enable_interrupts(); // only 2 simple C commands while ISRs disabled
        StoringData = 1; // message to ISR to start storing data
        
        while (StoringData) { // wait until ISR says data storing is done
            ; // do nothing
        }
        
        for (i=0; i<PLOTPTS; i++) { // send plot data to MATLAB
            // when first number sent = 1, MATLAB knows we’re done
            sprintf(message, "%d %d %d\r\n", PLOTPTS-i, ADCarray[i], REFarray[i]);
            NU32DIP_WriteUART1(message);
        }
    }  // infinite loop

    return 0;
}

