# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import serial
ser = serial.Serial('/dev/ttyUSB0',230400)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\nb: Read current sensor (mA) \nc: Read encoder (ticks) \nd: Read encoder (degrees) \ne: Reset encoder count (ticks) \
           \nq: Quit & set PIC32 to IDLE mode \nr: Current mode of PIC32 \nx: Dummy Add to Numbers') # '\t' is a tab
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'b'): # Read current sensor (mA)
        n_str = ser.read_until(b'\n'); 
        n_int = float(n_str) # turn it into an float
        print('Current sensor: ' + str(n_int) + ' mA' + '\n') # print it to the screen
    elif (selection == 'c'): # Read encoder count (ticks)
        n_str = ser.read_until(b'\n'); 
        n_int = int(n_str) # turn it into an int
        print('Got back encoder count: ' + str(n_int) + '\n') # print it to the screen

    elif (selection == 'd'):
        n_str = ser.read_until(b'\n'); 
        n_int = int(n_str) # turn it into an int
        print('Got back encoder degrees: ' + str(n_int) + '\n') # print it to the screen

    elif (selection == 'e'): # Reset encoder count
        print('Reset encoder count ' + '\n') # print it to the screen

    elif (selection == 'q'): # Quit & set PIC32 to IDLE mode
        print('Exiting client and setting PIC32 to IDLE mode')
        ser.write(('q'+'\n').encode()); # send the number to PIC

        has_quit = True; # exit client
        # be sure to close the port
        ser.close()

    elif (selection == 'r'): # Current mode of PIC32
        n_str = ser.read_until(b'\n'); 
        n_int = int(n_str) # turn it into an int
        if (n_int == 0):
            curr_mode = "IDLE"
        elif (n_int == 1):
            curr_mode = "PWM"
        elif (n_int == 2):
            curr_mode = "ITEST"
        elif (n_int == 3):
            curr_mode = "HOLD"
        elif (n_int == 4):
            curr_mode = "TRACK"
        print('PIC32 mode is: ' + curr_mode + ' (' + str(n_int) + ')\n') # print it to the screen

    elif (selection == 'x'): # add two numbers and return
        # First number
        n_str = input('Enter 1ste number: ') # get the number to send
        n_int = int(n_str) # turn it into an int
        print('number = ' + str(n_int)) # print it to the screen to double check
        ser.write((str(n_int)+'\n').encode()); # send the number to PIC

        # Second number
        n_str2 = input('Enter 2nd number: ') # get the number to send
        n_int2 = int(n_str2) # turn it into an int
        print('number = ' + str(n_int2)) # print it to the screen to double check
        ser.write((str(n_int2)+'\n').encode()); # send the number

        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = int(n_str) # turn it into an int
        print('Got back: ' + str(n_int) + '\n') # print it to the screen

    else:
        print('Invalid Selection ' + selection_endline)



