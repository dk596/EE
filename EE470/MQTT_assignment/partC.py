from lib.simple import MQTTClient
import machine
import network
import time
import secrets

# --- 1. Wi-Fi Connection ---
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect(secrets.WIFI_SSID, secrets.WIFI_PASSWORD)

while not wlan.isconnected():
    time.sleep(0.5)
    print("Connecting to Wi-Fi...")

print("Wi-Fi is ready! IP:", wlan.ifconfig()[0])

# --- 2. Hardware Setup ---
pot_pin = machine.ADC(0)

# Physical pin D1 is GPIO 5 (External LED)
led_pin = machine.Pin(5, machine.Pin.OUT)
led_pin.value(0) 

# Physical pin D2 is GPIO 4 (Switch with pull-up)
switch_pin = machine.Pin(4, machine.Pin.IN, machine.Pin.PULL_UP)

# --- 3. MQTT Callback with Exact Matching ---
def mqtt_callback(topic, msg):
    print("\n--- INCOMING MESSAGE ---")
    try:
        safe_topic = topic.decode('utf-8').strip()
        safe_msg = msg.decode('utf-8').strip()
        
        print("Topic:", safe_topic, "| Payload:", safe_msg)
        
        if safe_topic == "testtopic/temp/inTopic":
            if safe_msg == "1":
                led_pin.value(1)
                print(">>> LED Turned ON (Exact Match)")
            elif safe_msg == "0":
                led_pin.value(0)
                print(">>> LED Turned OFF (Exact Match)")
            else:
                print(">>> IGNORED: Payload was not strictly '1' or '0'")
    except Exception as e:
        print("Callback error:", e)

# --- 4. Secure MQTT Settings ---
IN_TOPIC = b"testtopic/temp/inTopic"
OUT_TOPIC = b"testtopic/temp/outTopic/data"

client = MQTTClient(
    client_id=secrets.MQTT_CLIENT_ID, 
    server=secrets.MQTT_BROKER, 
    port=8883, 
    user=secrets.MQTT_USER, 
    password=secrets.MQTT_PASSWORD, 
    ssl=True, 
    ssl_params={"server_hostname": secrets.MQTT_BROKER} 
)

client.set_callback(mqtt_callback)

# --- 5. Connect and Main Multitasking Loop ---
try:
    print("Connecting to secure HiveMQ broker...")
    client.connect()
    print("Connected!")
    
    client.subscribe(IN_TOPIC)
    print("Subscribed to", IN_TOPIC.decode())
    
    # State tracking variables
    last_switch_state = switch_pin.value()
    waiting_for_zero = False
    timer_start = 0
    
    # Telemetry interval timer (15 seconds = 15000 milliseconds)
    telemetry_timer = time.ticks_ms()
    TELEMETRY_INTERVAL = 15000
    
    while True:
        # 1. Non-blocking check for incoming LED messages
        try:
            client.check_msg()
        except OSError:
            pass
        
        # 2. Read current switch state
        current_switch_state = switch_pin.value()
        
        # 3. Detect switch "pressed and released" (transition from 0 to 1)
        if last_switch_state == 0 and current_switch_state == 1:
            client.publish(OUT_TOPIC, b"1")
            print("Switch released! Published 1 to outTopic/data")
            
            # Start the 5-second zero-reset countdown
            waiting_for_zero = True
            timer_start = time.ticks_ms()
            
        last_switch_state = current_switch_state
        
        # 4. Check if 5 seconds have elapsed since switch release
        if waiting_for_zero and time.ticks_diff(time.ticks_ms(), timer_start) >= 5000:
            client.publish(OUT_TOPIC, b"0")
            print("5 seconds elapsed. Published 0 to outTopic/data")
            waiting_for_zero = False 
            
        # 5. Check if 15 seconds have passed to send periodic telemetry data
        if time.ticks_diff(time.ticks_ms(), telemetry_timer) >= TELEMETRY_INTERVAL:
            telemetry_timer = time.ticks_ms() # Reset interval timer
            
            # Read sensor values
            adc_value = pot_pin.read()
            voltage = adc_value * (3.3 / 1023.0)
            sw_status = "Pressed" if current_switch_state == 0 else "Unpressed"
            
            # Format payload string and publish
            payload = "Voltage: {:.2f}V | Switch: {}".format(voltage, sw_status)
            client.publish(OUT_TOPIC, payload.encode('utf-8'))
            print("Periodic Telemetry Sent:", payload)

        # Small delay to keep the CPU stable and protect the SSL buffer
        time.sleep_ms(50)

except Exception as e:
    print("An error occurred:", e)
