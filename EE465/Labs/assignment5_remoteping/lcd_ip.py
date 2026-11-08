#!/usr/bin/env python3
import time, os, subprocess, board, digitalio
import adafruit_character_lcd.character_lcd as charlcd

# 1. Hardware Setup
lcd_pins = [digitalio.DigitalInOut(p) for p in (board.D25, board.D24, board.D23, board.D17, board.D27, board.D22)]
lcd = charlcd.Character_LCD_Mono(*lcd_pins, 16, 2)

btn = digitalio.DigitalInOut(board.D26)
btn.direction = digitalio.Direction.INPUT
btn.pull = digitalio.Pull.UP

# 2. Data Helpers
def get_mac():
    path = '/sys/class/net/wlan0/address' if os.path.exists('/sys/class/net/wlan0/address') else '/sys/class/net/eth0/address'
    return open(path).read().strip().upper().replace(":", "")

def get_ip():
    try:
        raw_output = subprocess.check_output(['hostname', '-I']).decode('utf-8')
        return raw_output.split()[0]
    except Exception:
        return "No Connection"

# 3. Main Loop
while True:
    new_text = f"MAC Address:\n{get_mac()}" if not btn.value else f"IP Address:\n{get_ip()}"
    
    if lcd.message != new_text:
        lcd.clear()
        lcd.message = new_text
        
    time.sleep(0.2)
