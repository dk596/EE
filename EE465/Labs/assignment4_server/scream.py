#!/usr/bin/env python3
import os
import subprocess
from signal import pause
from gpiozero import Button

AUDIO_FILE = os.path.expanduser("~/ftp/files/Wilhelm_4.wav")

def play_scream():
    print("Switch triggered! Playing Wilhelm_4.wav...")
    # Explicitly routing to card 1, device 0 avoids ALSA default routing errors
    subprocess.Popen(["aplay", "-D", "plughw:1,0", AUDIO_FILE])

# GPIO 17 (Pin 11) with internal pull-up resistor
switch = Button(17, pull_up=True, bounce_time=0.2)
switch.when_pressed = play_scream

print(f"Monitoring switch on GPIO 17... (Routing to plughw:1,0)")
pause()
