from turntablecontrol import TurnTableControl # to control motor
from time import sleep, perf_counter # for waiting and time measure
import numpy as np # for array math
import matplotlib.pyplot as plt # for plotting


motor = TurnTableControl('/dev/tty.usbserial', tableGearRatio=1) # test motor only
data = np.zeros((100, 3)) # prepare data

# speed-up
motor.setSpeed(1000)
sleep(3)
# slow down
motor.setSpeed(1)

# start measure
startTime = perf_counter()
motor.position = 0
for iData in range(data.shape[0]):
    data[iData, 1] = motor.position
    data[iData, 2] = motor.rate
    data[iData, 0] = perf_counter()-startTime
    sleep(0.1)

motor.setSpeed(0)

# prepare figure
_, ax = plt.subplots(figsize=(10, 8))
# plot
ax.plot(data[:, 0], data[:, 1], label='position [deg]')
ax.plot(data[:, 0], data[:, 2], label='rate [deg/s]')
# annotate
ax.set_xlabel('time [s]')
ax.grid()
ax.legend()

plt.show()