# Write your code here :-)
import board
import digitalio
import time

mode = digitalio.DigitalInOut(board.SCL)
mode.direction = digitalio.Direction.INPUT
mode.pull = digitalio.Pull.UP

start = digitalio.DigitalInOut(board.SDA)
start.direction = digitalio.Direction.INPUT
start.pull = digitalio.Pull.UP

motor = digitalio.DigitalInOut(board.D5)
motor.direction = digitalio.Direction.OUTPUT

MODE_40 = True
MODE_25 = False

TIMER_25 = [
    (0,     0.15),
    (0.3,   0.15),
    (0.6,   0.15),
    (15,    1.0),
    (20,    0.5),
    (21,    0.5),
    (22,    0.5),
    (23,    0.5),
    (24,    0.5),
    (25,    5.0),
]
TIMER_40 = [
    (0,     0.15),
    (0.3,   0.15),
    (0.6,   0.15),
    (15,    0.25),
    (15.5,  0.25),
    (30,    1.0),
    (35,    0.5),
    (36,    0.5),
    (37,    0.5),
    (38,    0.5),
    (39,    0.5),
    (40,    5.0),
]

prevStartValue = True
currentTimer = []

vibrating = False
vibrateUntil = -1

lastSecond = -1

while True:
    currentTime = time.monotonic()
    currentSecond = int(currentTime)

    if currentSecond > lastSecond:
        print("TIMER: " + str(currentTimer))
        print("MODE:  " + str(mode.value))
        print("START: " + str(start.value))
        print("MOTOR: " + str(motor.value))
        print("prevStartValue: " + str(prevStartValue))
        print()
        lastSecond = currentSecond

    # Stop vibrating, if necessary
    if vibrating and vibrateUntil < currentTime:
        motor.value = False
    # Check for a new timer
    if prevStartValue and start.value is not prevStartValue:
        # this means the button was pressed
        motor.value = False  # Stop the motor immediately

        if mode.value == MODE_25:
            currentTimer = [(vib[0] + currentTime, vib[1]) for vib in TIMER_25]
        else:
            currentTimer = [(vib[0] + currentTime, vib[1]) for vib in TIMER_40]
    # if there's no timer running, skip timer actions
    if not currentTimer:
        continue
    # perform vibration actions
    nextVib = currentTimer[0]
    if nextVib[0] < currentTime:
        currentTimer.pop(0)
        vibrating = True
        vibrateUntil = currentTime + nextVib[1]
        motor.value = True

    prevStartValue = start.value
    time.sleep(0.05)
