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
    print('\nc: Read encoder (ticks) \nd: Read encoder (degrees) \ne: Reset encoder count (ticks) \nx: Dummy Add to Numbers \nq: Quit') # '\t' is a tab
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'c'): # Read encoder count (ticks)
        n_str = ser.read_until(b'\n'); 
        n_int = int(n_str) # turn it into an int
        print('Got back encoder count: ' + str(n_int) + '\n') # print it to the screen
    elif (selection == 'd'):
        n_str = ser.read_until(b'\n'); 
        n_int = int(n_str) # turn it into an int
        print('Got back encoder degrees: ' + str(n_int) + '\n') # print it to the screen
    elif (selection == 'e'): # Reset encoder count
        print('Reset encoder count ' + '\n') # print it to the screen
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
    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()
    else:
        print('Invalid Selection ' + selection_endline)



