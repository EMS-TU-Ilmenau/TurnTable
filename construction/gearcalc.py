import math # for pi

'''
Table gear
'''
pitchDiameterTable = 800 # pitch diameter in mm
maxPrintSize = 300 # maximum 3D printeable size (not really, but close enough for circumference >> print size)
nTeethArc = 30 # number of teeth on printed arc

pitchCircumference = math.pi*pitchDiameterTable
nArcs = int(pitchCircumference/maxPrintSize)
print(f'Number of arcs: {nArcs}')

arcLength = pitchCircumference/nArcs
print(f'Arc length: {arcLength} mm')

circPitch = arcLength/nTeethArc
print(f'Circular pitch: {circPitch} mm')

module = circPitch/math.pi
print(f'Module = {module}')


'''
Motor gear
'''
nTeethMotor = 12
pitchDiameterMotor = module*nTeethMotor
print(f'Motor gear pitch diameter: {pitchDiameterMotor}')

gearRatio = pitchDiameterMotor/pitchDiameterTable
print(f'Gear ratio: {gearRatio} -> Torque gain: {1/gearRatio}')