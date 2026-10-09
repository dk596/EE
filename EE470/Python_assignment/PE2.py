#!/usr/bin/env python3
import subprocess
from signal import pause
from gpiozero import Button

# Target host to test
TARGET_HOST = "google.com"

# GPIO 27 with internal pull-up enabled (active-low to Ground)
# bounce_time=0.1 prevents contact chatter from sending multiple pings
button = Button(27, pull_up=True, bounce_time=0.1)


def ping_host():
    print(f"\n[Switch Pressed] Pinging {TARGET_HOST}...")

    # -c 1: send exactly 1 packet
    # -W 2: timeout after 2 seconds if no reply arrives
    # stdout=subprocess.PIPE captures the output for processing
    # stderr=subprocess.PIPE captures any error messages
    # text=True ensures the output is returned as a string instead of bytes
    result = subprocess.run(
        ["ping", "-c", "1", "-W", "2", TARGET_HOST],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )

    if result.returncode == 0:
        # Extract the round-trip time line from ping output
        for line in result.stdout.splitlines():
            if "time=" in line:
                print(f"SUCCESS: {line.strip()}")
                return
        print("SUCCESS: Host replied.")
    else:
        print("FAILED: Host unreachable or request timed out.")


# Attach callback: triggers only on press
button.when_pressed = ping_host

print("System ready. Press the switch on GPIO 27 to send a ping (Ctrl+C to quit)...")
pause()
