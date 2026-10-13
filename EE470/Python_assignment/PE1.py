#!/usr/bin/env python3
from gpiozero import Button
from signal import pause

# bounce_time=0.05 ignores extra bounces for 50 milliseconds
button = Button(27, bounce_time=0.05)

button.when_pressed = lambda: print("Button Pressed") # HIGH to LOW
button.when_released = lambda: print("Button Released") # LOW to HIGH

pause()
