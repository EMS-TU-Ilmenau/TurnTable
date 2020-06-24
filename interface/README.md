# TurnTableControl
Python interface for the turntable controller board

## Getting started
Install this package via `pip install .`

**Example**
```python
from turntablecontrol import TurnTableControl

table = TurnTableControl('COM7', axisID=6)
table.setSpeed(500) # power control -1000...1000
print(f'Current position: {table.position} deg, rate: {table.rate} deg/s')
```