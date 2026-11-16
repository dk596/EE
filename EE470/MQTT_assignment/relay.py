import paho.mqtt.client as mqtt
import requests
import re
import secrets  # Imports configuration from secrets.py

# Default tracking states for database fields
current_voltage = 0.0
current_switch = "Unpressed"
current_led = 0

def push_to_database():
    """Helper function to package current states and send them to Hostinger."""
    params = {
        'voltage': current_voltage,
        'switch_state': current_switch,
        'led_state': current_led
    }
    try:
        response = requests.get(secrets.INSERT_URL, params=params)
        print(f"[Hostinger Insert Response] HTTP {response.status_code}: {response.text}")
    except Exception as e:
        print("[HTTP Error] Failed to reach Hostinger server:", e)

def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print("Successfully connected to HiveMQ Cloud broker!")
        # Subscribe to both incoming control and outgoing telemetry channels
        client.subscribe(secrets.TOPIC_TELEMETRY)
        client.subscribe(secrets.TOPIC_LED_CONTROL)
        print(f"Subscribed to topics:\n  - {secrets.TOPIC_TELEMETRY}\n  - {secrets.TOPIC_LED_CONTROL}")
    else:
        print(f"Connection failed with code {rc}")

def on_message(client, userdata, msg):
    global current_voltage, current_switch, current_led
    payload = msg.payload.decode('utf-8').strip()
    print(f"\n[MQTT Message Received] Topic: {msg.topic} | Payload: {payload}")

    # --- Case A: Incoming LED Control Command (inTopic) ---
    if msg.topic == secrets.TOPIC_LED_CONTROL:
        if payload == "1":
            current_led = 1
            print(">>> LED State updated to ON (1)")
        elif payload == "0":
            current_led = 0
            print(">>> LED State updated to OFF (0)")
        
        # Log the state change immediately to the database
        push_to_database()

    # --- Case B: Outgoing Telemetry & Button Events (outTopic/data) ---
    elif msg.topic == secrets.TOPIC_TELEMETRY:
        # 1. Parse periodic telemetry string (e.g., "Voltage: 2.45V | Switch: Unpressed")
        if "Voltage:" in payload:
            try:
                v_match = re.search(r"Voltage:\s*([\d\.]+)V", payload)
                if v_match:
                    current_voltage = float(v_match.group(1))
                
                s_match = re.search(r"Switch:\s*(\w+)", payload)
                if s_match:
                    current_switch = s_match.group(1)
            except Exception as e:
                print("Error parsing telemetry string:", e)

        # 2. Parse direct button event payloads ("1" or "0")
        elif payload == "1":
            current_switch = "Pressed"
        elif payload == "0":
            current_switch = "Unpressed"

        # Log the telemetry update to the database
        push_to_database()

def main():
    # Initialize MQTT client with modern v2 callback API
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="pc_relay_node")
    client.username_pw_set(secrets.MQTT_USER, secrets.MQTT_PASSWORD)
    
    # Enable TLS/SSL encryption required by HiveMQ Cloud
    client.tls_set()
    
    client.on_connect = on_connect
    client.on_message = on_message

    print(f"Connecting to secure broker {secrets.MQTT_BROKER}:{secrets.MQTT_PORT}...")
    client.connect(secrets.MQTT_BROKER, secrets.MQTT_PORT, keepalive=60)
    
    # Enter blocking loop to continuously process incoming MQTT messages
    client.loop_forever()

if __name__ == "__main__":
    main()
