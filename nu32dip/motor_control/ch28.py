# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import serial
from ch28_read_plot_matrix import read_plot_matrix
from genref import genRef

ser = serial.Serial('/dev/ttyUSB0',230400)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('\nPIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\nb: Read current sensor (mA) \nc: Read encoder (ticks) \nd: Read encoder (degrees) \
           \ne: Reset encoder count (ticks) \nf: Set PWM (-100 to 100) duty cycle \
           \ng: Set current gains \nh: Get current gains \ni: Set position gains \
           \nj: Get position gains \nk: Test current control \nl: Go to angle (deg) \
           \nm: Load step trajectory \nn: Load cubic trajectory \no: Execute trajectory \
           \np: Unpower the motor \nq: Quit & set PIC32 to IDLE mode \nr: Current mode of PIC32 \
           \nx: Dummy Add to Numbers') # '\t' is a tab

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

    elif (selection == 'f'): # Set PWM (-100 to 100) and direction
        n_str = input('Enter the PWM duty cycle [%] with +- as the direction: ') # get the number to send
        n_int = int(n_str) # turn it into an int
        print('PWM duty_cycle = ' + str(n_int) + '%') # print it to the screen to double check
        ser.write((str(n_int)+'\n').encode()); # send the number to PIC

    elif (selection == 'g'): # Set current gains
        # First number
        n_str = input('Enter Kp: ') # get the number to send
        n_int = float(n_str) # turn it into an int
        ser.write((str(n_int)+'\n').encode()); # send the number to PIC

        # Second number
        n_str2 = input('Enter Ki: ') # get the number to send
        n_int2 = float(n_str2) # turn it into an int
        ser.write((str(n_int2)+'\n').encode()); # send the number

    elif (selection == 'h'): # Get current gains
        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = float(n_str) # turn it into an int
        print('Kp: ' + str(n_int) + '\n') # print it to the screen

        n_str2 = ser.read_until(b'\n');  # get the incremented number back
        n_int2 = float(n_str2) # turn it into an int
        print('Ki: ' + str(n_int2) + '\n') # print it to the screen

    elif (selection == 'i'): # Set position gains
        # First number
        n_str = input('Enter Kp: ') # get the number to send
        n_int = float(n_str) # turn it into an float
        ser.write((str(n_int)+'\n').encode()); # send the number to PIC

        # Second number
        n_str = input('Enter Kd: ') # get the number to send
        n_int = float(n_str) # turn it into an float
        ser.write((str(n_int)+'\n').encode()); # send the number to PIC

        # Third number
        n_str2 = input('Enter Ki: ') # get the number to send
        n_int2 = float(n_str2) # turn it into an float
        ser.write((str(n_int2)+'\n').encode()); # send the number

    elif (selection == 'j'): # Get position gains
        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = float(n_str) # turn it into an float
        print('Kp_pos: ' + str(n_int) + '\n') # print it to the screen

        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = float(n_str) # turn it into an float
        print('Kd_pos: ' + str(n_int) + '\n') # print it to the screen

        n_str2 = ser.read_until(b'\n');  # get the incremented number back
        n_int2 = float(n_str2) # turn it into an float
        print('Ki_pos: ' + str(n_int2) + '\n') # print it to the screen

    elif (selection == 'k'): # Test current control
        print('Testing current controller!' + '\n')
        read_plot_matrix()
        print('Done testing!' + '\n')

    elif (selection == 'l'): # Go to angle (deg)
        # First number
        n_str = input('Enter angle: ') # get the number to send
        n_int = float(n_str) # turn it into an float
        ser.write((str(n_int)+'\n').encode()); # send the number to PIC

        print('Going to angle...' + '\n')
        read_plot_matrix()
        print('Reached desired angle!' + '\n')

    elif (selection == 'm'): # Load step trajectory
        print('You must enter from 0 - 10 seconds!' + '\n')
        traj = genRef('step')
        for i in traj:
            ser.write((str(i)+'\n').encode()); # send the number to PIC

        print('Loaded step trajectory!' + '\n')

    elif (selection == 'n'): # Load cubic trajectory
        print('You must enter from 0 - 10 seconds!' + '\n')
        traj = genRef('cubic')
        for i in traj:
            ser.write((str(i)+'\n').encode()); # send the number to PIC

        print('Loaded cubic trajectory!' + '\n')

    elif (selection == 'o'): # Execute trajectory

        print('Tracking trajectory...' + '\n')
        read_plot_matrix()
        print('Done!' + '\n')

    elif (selection == 'p'): # Unpower the motor
        print('Motor unpowered!' + '\n')

    elif (selection == 'q'): # Quit & set PIC32 to IDLE mode
        print('Exiting client and setting PIC32 to IDLE mode'  + '\n')

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



