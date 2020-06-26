import serial # (pyserial) for communicating with the controller board
import time # for sleeping


class TurnTableControl:
    '''
    Class to control the TurnTable project board
    '''
    def __init__(self, port, axisID=6, startPos=None, rotorSteps=12, motorGearRatio=38.6, tableGearRatio=20):
        '''
        :param port: serial port to communicate with the motor controller
            OR already opened serial object
        :param axisID: motor controller board identifier number
        :param startPos: optional: redefine current physical position (angle) in degrees
        :param motorSteps: number of steps per rotor revolution
        :param motorGearRatio: ratio:1 of the motor gear
        :param tableGearRatio: ratio:1 of the table gear
        '''
        self.id = axisID
        self.rotorSteps = rotorSteps
        self.motorGearRatio = motorGearRatio
        self.tableGearRatio = tableGearRatio

        # connect
        self.dev = None
        if isinstance(port, serial.Serial):
            self.dev = port
        elif isinstance(port, str):
            self.connect(port)
        else:
            raise TypeError('port must be either a string or serial.Serial object')

        # initial settings
        if startPos is not None:
            self.position = startPos
    

    def __del__(self):
        # close connection properly when destructing the instance
        self.disconnect()
    

    def connect(self, port):
        '''
        Connect to serial port
        '''
        try:
            self.dev = serial.Serial(port, 9600, timeout=2)
        except:
            raise IOError('Cannot connect to stepper motor controller hub')
    

    def disconnect(self):
        '''
        Disconnect from serial port
        '''
        if self.dev:
            self.dev.close()
    

    def send(self, cmd):
        '''
        sends a command via the port to the motor controller

        :param cmd: command string (without line endings) to send
        :returns: when the command was a query, the response string is returned
        '''
        self.dev.write(bytes(cmd+'\n', 'ascii'))
        if '?' in cmd:
            resp = self.dev.readline()
            return str(bytes(filter(lambda c: c > 32, resp)), 'ascii') # remove control chars
        return None
    

    @property
    def axStr(self):
        '''
        :returns: complete axis name (prefix + id)
        '''
        return f'AX{self.id}'
    

    def setSpeed(self, speed):
        '''
        Sets the rotor power
        
        :param speed: number between -1000...1000
        '''
        self.send(f'{self.axStr}:VEL {-speed}')
    

    @property
    def tableRevSteps(self):
        '''
        :returns: number of steps per table revolution
        '''
        return self.rotorSteps*self.motorGearRatio*self.tableGearRatio
    

    @property
    def position(self):
        '''
        :returns: current table angle in degrees
        '''
        rotorSteps = -int(self.send(f'{self.axStr}:POS?'))
        # convert rotor step position to table position
        tablePos = 360*rotorSteps/self.tableRevSteps
        return tablePos%360
    

    @position.setter
    def position(self, val):
        '''
        Defines the current table angle in degrees
        '''
        # convert table position to rotor steps
        rotorSteps = round(val*self.tableRevSteps/360)
        # send to board
        self.send(f'{self.axStr}:POS {-rotorSteps}')


    @property
    def rate(self):
        '''
        :returns: current table velocity in degrees/s
        '''
        stepPeriod = int(self.send(f'{self.axStr}:PER?'))*128 # in us
        if stepPeriod == 0:
            return -1
        stepPeriod *= 1e-6 # in s
        stepRate = 1/stepPeriod # in steps/s
        angleRate = 360*stepRate/self.tableRevSteps
        return angleRate
