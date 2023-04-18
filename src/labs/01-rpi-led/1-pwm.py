#!/usr/bin/python

from gpiozero import PWMLED
from time import sleep

led = PWMLED(23)

step = 1
led.value = 0
value = 0

while True:
    if value == 100:
        step = -1
    elif value == 0:
        step = +1
    
    value += step
    led.value = value / 100.0

    # print(value, led.value)
    sleep(0.01)