#!/usr/bin/env python3
import socket
import subprocess
import threading
import time
from gpiozero import Button, LED

# Hardware Pin Definitions (BCM numbering)
PIN_WIFI_OFF = 27  # Physical Pin 13
PIN_WIFI_ON = 22  # Physical Pin 15
PIN_LED = 17  # Physical Pin 11

led = LED(PIN_LED)
btn_wifi_off = Button(PIN_WIFI_OFF, pull_up=True, bounce_time=0.1)
btn_wifi_on = Button(PIN_WIFI_ON, pull_up=True, bounce_time=0.1)


def has_valid_wlan_ip(interface="wlan0") -> bool:
    """Checks if the wireless interface has an assigned, routable IPv4 address."""
    cmd = ["ip", "-4", "-brief", "addr", "show", interface]
    try:
        res = subprocess.run(cmd, capture_output=True, text=True, check=False)
        if res.returncode != 0:
            return False
        # Output format: "wlan0 UP 192.168.1.50/24 ..."
        parts = res.stdout.strip().split()
        if len(parts) >= 3 and parts[1].upper() == "UP":
            ip_str = parts[2].split("/")[0]
            # Exclude loopback/link-local/null addresses
            if (
                ip_str
                and not ip_str.startswith("127.")
                and not ip_str.startswith("169.254.")
            ):
                return True
    except Exception:
        pass
    return False


def set_wifi_state(enable: bool):
    """Controls the Wi-Fi radio via NetworkManager / rfkill without dropping SSH over Ethernet."""
    action = "on" if enable else "off"
    print(f"\n[SWITCH TRIGGERED] Setting Wi-Fi radio to: {action.upper()}...")

    # Method 1: NetworkManager (standard on modern Raspberry Pi OS Bookworm/Bullseye)
    res = subprocess.run(
        ["nmcli", "radio", "wifi", action], capture_output=True, text=True
    )
    if res.returncode != 0:
        # Fallback for systems without NetworkManager: rfkill
        rfkill_action = "unblock" if enable else "block"
        subprocess.run(
            ["sudo", "rfkill", rfkill_action, "wifi"], capture_output=True
        )

    print(f"[STATUS] Wi-Fi hardware switched {action.upper()}.")


def wifi_monitor_loop():
    """Background watchdog:

    - If Wi-Fi is DOWN / NO IP: Blinks the LED continuously.
    - If Wi-Fi is UP & HAS IP: Turns the LED solid ON (or solid OFF if you prefer).
    """
    while True:
        if not has_valid_wlan_ip("wlan0"):
            # No valid Wi-Fi IP: Rapid blink state
            led.on()
            time.sleep(0.2)
            led.off()
            time.sleep(0.2)
        else:
            # Valid IP acquired: Keep steady OFF (or ON to indicate ready)
            led.off()
            time.sleep(1.0)


# Bind interrupts
btn_wifi_off.when_pressed = lambda: set_wifi_state(enable=False)
btn_wifi_on.when_pressed = lambda: set_wifi_state(enable=True)

if __name__ == "__main__":
    print("=" * 60)
    print("  Raspberry Pi Wi-Fi Hardware Controller & State Monitor")
    print(f"  - Press Switch on GPIO {PIN_WIFI_OFF} -> DISABLE Wi-Fi")
    print(f"  - Press Switch on GPIO {PIN_WIFI_ON}  -> RESTORE Wi-Fi")
    print(f"  - LED on GPIO {PIN_LED}            -> Blinks when NO IP")
    print("=" * 60)

    # Start the IP / link monitor in a background daemon thread
    monitor_thread = threading.Thread(target=wifi_monitor_loop, daemon=True)
    monitor_thread.start()

    try:
        # Keep main thread alive
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nShutting down controller...")
        led.close()
