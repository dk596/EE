from lib.simple import MQTTClient
import machine
import network
import time
import secrets # Pulls in your hidden variables

# --- 1. Wi-Fi Connection ---
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect(secrets.WIFI_SSID, secrets.WIFI_PASSWORD)

while not wlan.isconnected():
    time.sleep(0.5)
    print("Connecting to Wi-Fi...")

print("Wi-Fi is ready! IP:", wlan.ifconfig()[0])

# --- 2. Secure MQTT Settings ---
TOPIC = b"testtopic/temp/outTopic/data" 

client = MQTTClient(
    client_id=secrets.MQTT_CLIENT_ID, 
    server=secrets.MQTT_BROKER, 
    port=8883, 
    user=secrets.MQTT_USER, 
    password=secrets.MQTT_PASSWORD, 
    ssl=True, 
    ssl_params={"server_hostname": secrets.MQTT_BROKER} 
)

# --- 3. Hardware Setup ---
pot_pin = machine.ADC(0)
switch_pin = machine.Pin(4, machine.Pin.IN, machine.Pin.PULL_UP)

# --- 4. Connect and Publish Loop ---
try:
    print("Connecting to secure HiveMQ broker...")
    client.connect()
    print("Connected!")
    
    while True:
        adc_value = pot_pin.read()
        voltage = adc_value * (3.3 / 1023.0)
        switch_state = "Pressed" if switch_pin.value() == 0 else "Unpressed"
        
        payload_text = "Voltage: {:.2f}V | Switch: {}".format(voltage, switch_state)
        payload_bytes = payload_text.encode("utf-8")
        
        client.publish(TOPIC, payload_bytes)
        print("Published to {}: {}".format(TOPIC.decode(), payload_text))
        
        time.sleep(15)

except Exception as e:
    print("An error occurred:", e)
