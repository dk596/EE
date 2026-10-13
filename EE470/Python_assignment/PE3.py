#!/usr/bin/env python3
import socket
import struct
import threading
import time
from gpiozero import LED

# LED connected to BCM GPIO 17 (Physical Pin 11)
led = LED(17)

# Flash duration in seconds
FLASH_DURATION = 0.3

# Lock to avoid conflicting timer threads if pinged rapidly
timer_lock = threading.Lock()
off_timer = None


def turn_off_led():
    global off_timer
    led.off()
    with timer_lock:
        off_timer = None


def flash_led():
    global off_timer
    with timer_lock:
        led.on()
        # Reset or start turn-off timer
        if off_timer is not None:
            off_timer.cancel()
        off_timer = threading.Timer(FLASH_DURATION, turn_off_led)
        off_timer.start()


def listen_for_pings():
    # IPPROTO_ICMP = 1
    # Raw socket captures all incoming IPv4 ICMP packets
    s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)

    print("Listening for incoming ICMP pings... (Press Ctrl+C to stop)")

    try:
        while True:
            # Receive raw packet data (buffer size 1024 bytes)
            packet, addr = s.recvfrom(1024)

            # The standard IPv4 header is 20 bytes long.
            # Byte 20 is the start of the ICMP header:
            #   Byte 20: ICMP Type (8 = Echo Request, 0 = Echo Reply)
            #   Byte 21: ICMP Code (0)
            icmp_header = packet[20:24]
            icmp_type, icmp_code, _ = struct.unpack("!BBH", icmp_header)

            # Type 8 is an incoming ICMP Echo Request ("ping")
            if icmp_type == 8:
                sender_ip = addr[0]
                print(f"[PING RECEIVED] From: {sender_ip}")
                flash_led()

    except KeyboardInterrupt:
        print("\nStopping listener...")
    finally:
        s.close()
        led.off()


if __name__ == "__main__":
    listen_for_pings()
