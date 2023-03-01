#include "nu32dip.h"           // constants, funcs for startup and UART

#define NUMSAMPS 1000 // number of points in waveform
#define PLOTPTS 200 // number of data points to plot
#define DECIMATION 10 // plot every 10th point
#define SAMPLE_TIME 10       // 10 core timer ticks = 480 ns // HOW LONG SWITCH SHOULD BE CLOSED TO CHARGE CAPACITOR FOR ADC
#define INTEGRAL_LIMIT 400.0 // Limit integral windup 


static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS]; // measured values to plot
static volatile int REFarray[PLOTPTS]; // reference values to plot
static volatile int StoringData = 0; // if this flag = 1, currently storing
// plot data
static volatile float Kp = 0.0, Ki = 0.0; // control gains - Changed in python ch24.py file

// Function prototypes
void makeWaveform();
unsigned int adc_sample_convert(int pin);


void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {

    static int counter = 0; // initialize counter once
    static int plotind = 0; // index for data arrays; counts up to PLOTPTS
    static int decctr = 0; // counts to store data one every DECIMATION

    // READ ADC value of phototransistor
    unsigned int adcval = adc_sample_convert(1); // Return 0-1023 representing voltage from photoresistor

    // PI Control
    int error = Waveform[counter] - adcval; // Calculate error
    static int integral_error = 0;
    integral_error += error;
    // Limit integral windup
    if (integral_error > INTEGRAL_LIMIT){
        integral_error = INTEGRAL_LIMIT;
    } else if (integral_error < -INTEGRAL_LIMIT)
    {
        integral_error = -INTEGRAL_LIMIT;
    }
    float u = Kp*error + Ki*integral_error;
    float unew = u + 50.0; // Add 50% is feed forward control added to PI
    if (unew > 100.0) { // 100% duty cycle
        unew = 100.0;
    } else if (unew < 0.0) {
        unew = 0.0;
    }
    // insert line(s) to set OC1RS
    OC1RS = (unsigned int) ((unew/100.0) * PR3); // Send control

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
    int i = 0, center = 1023/2, A = 1023/4; // square wave, fill in center value and amplitude
    for (i = 0; i < NUMSAMPS; ++i) {
        if ( i < NUMSAMPS/2) {
            Waveform[i] = center + A;
        } else {
            Waveform[i] = center - A;
        }
    }
}

unsigned int adc_sample_convert(int pin) { // sample & convert the value on the given 
                                           // adc pin the pin should be configured as an 
                                           // analog input in AD1PCFG
    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = pin;                // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + SAMPLE_TIME;
    while (_CP0_GET_COUNT() < finish_time) { 
      ;                                   // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
    return ADC1BUF0;                      // read the buffer with the result
}


int main(void) {
    NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    makeWaveform();

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

    // Initialize ADC
    ANSELAbits.ANSA1 = 0;                      // A1 as AN1 is an adc pin
    AD1CON3bits.ADCS = 1;                   // ADC clock period is Tad = 2*(ADCS+1)*Tpb =
                                            //                           2*3*12.5ns = 75ns < 83ns
    AD1CON1bits.ADON = 1;                   // turn on A/D converter

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

    char message[100]; // message to and from MATLAB
    float kptemp = 0, kitemp = 0; // temporary local gains
    int i = 0; // plot data loop counter

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

