#!/usr/bin/env python3
from gpiozero import LED
from time import sleep

# Assign GPIO 17 (Pin 11)
led = LED(17)

try:
    while True:
        led.on()
        sleep(0.5)
        led.off()
        sleep(0.5)
except KeyboardInterrupt:
    led.off()
