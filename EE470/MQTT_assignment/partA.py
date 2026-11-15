import machine
import time
import sys
import select

# Hardware Pin Definitions
# ESP8266 ADC only has one channel: 0
pot_pin = machine.ADC(0)                     

# D2 on NodeMCU (GPIO 4)
switch_pin = machine.Pin(4, machine.Pin.IN, machine.Pin.PULL_UP) 

# D1 on NodeMCU (GPIO 5)
led_pin = machine.Pin(5, machine.Pin.OUT)    

# Variables for timing
update_interval = 100 # milliseconds
last_update = time.ticks_ms()

# Ensure LED is off at start
led_pin.value(0) 

print("\nESP8266 Hardware Monitor Started")
print("Type '1' to turn LED ON, '0' to turn LED OFF")
print("-" * 40)

# Register standard input (keyboard/serial) for non-blocking reads
poll_obj = select.poll()
poll_obj.register(sys.stdin, select.POLLIN)

while True:
    # 1. Check for incoming Serial data
    # poll(0) checks instantly without waiting (non-blocking)
    if poll_obj.poll(0):
        incoming_char = sys.stdin.read(1)
        
        if incoming_char == '1':
            led_pin.value(1)
            print("\n>>> LED Turned ON (Received 1)")
        elif incoming_char == '0':
            led_pin.value(0)
            print("\n>>> LED Turned OFF (Received 0)")

    # 2. Non-blocking delay to read hardware states
    current_time = time.ticks_ms()
    if time.ticks_diff(current_time, last_update) >= update_interval:
        last_update = current_time

        # Read Potentiometer (ESP8266 ADC is 10-bit: 0-1023)
        adc_value = pot_pin.read()
        voltage = adc_value * (3.3 / 1023.0)

        # Read Switch
        # PULL_UP means unpressed = 1 (True), pressed = 0 (False).
        # We invert it so pressed = 1 for the terminal output.
        switch_state = 1 if switch_pin.value() == 0 else 0

        # 3. Print the formatted output
        # We use sys.stdout.write instead of print() because print() forces a newline
        output_string = "Potentiometer: {:.2f} V (Raw: {}) | Switch Pressed: {}      \r".format(voltage, adc_value, switch_state)
        sys.stdout.write(output_string)
