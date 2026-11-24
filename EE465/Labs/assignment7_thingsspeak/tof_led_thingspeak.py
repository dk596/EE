import time
import board
import digitalio
import adafruit_vl53l1x
import requests
from secrets import THINGSPEAK_WRITE_KEY2, TALKBACK_ID, TALKBACK_API_KEY

THINGSPEAK_URL = "https://api.thingspeak.com/update"
TALKBACK_EXECUTE_URL = f"https://api.thingspeak.com/talkbacks/{TALKBACK_ID}/commands/execute"
UPLOAD_INTERVAL = 20.0  

# Set up LED on GPIO 5 (Physical Pin 29)
led = digitalio.DigitalInOut(board.D5)
led.direction = digitalio.Direction.OUTPUT
led.value = False

# Set up VL53L1X on I2C
i2c = board.I2C()
vl53 = adafruit_vl53l1x.VL53L1X(i2c)
vl53.distance_mode = 2
vl53.timing_budget = 100
vl53.start_ranging()

latest_distance_cm = None
last_upload = time.monotonic() - UPLOAD_INTERVAL

def sync_thingspeak(distance_cm):
    """1. Check TalkBack and actuate LED first. 2. Post telemetry with the new LED state."""
    # Step 1: Check TalkBack queue and update LED immediately
    try:
        tb_res = requests.post(TALKBACK_EXECUTE_URL, data={"api_key": TALKBACK_API_KEY}, timeout=5)
        command = tb_res.text.strip()
        if command == "LED_ON":
            led.value = True
            print("-> TalkBack executed: LED turned ON")
        elif command == "LED_OFF":
            led.value = False
            print("-> TalkBack executed: LED turned OFF")
    except Exception as e:
        print(f"TalkBack check error: {e}")

    # Step 2: Grab the true current LED state right after actuation
    current_led_val = 1 if led.value else 0

    # Step 3: Post distance and the updated LED state to ThingSpeak
    payload = {
        "api_key": THINGSPEAK_WRITE_KEY2,
        "field1": distance_cm,
        "field2": current_led_val,
    }
    response = requests.post(THINGSPEAK_URL, data=payload, timeout=10)
    response.raise_for_status()
    print(f"Uploaded Dist: {distance_cm} cm | LED State: {current_led_val} | Reply: '{response.text.strip()}'")

print("Starting VL53L1X + LED Telemetry loop (Ctrl+C to stop)...")

try:
    while True:
        now = time.monotonic()

        if vl53.data_ready:
            dist = vl53.distance
            vl53.clear_interrupt()
            if dist is not None:
                latest_distance_cm = dist

        if (now - last_upload >= UPLOAD_INTERVAL) and (latest_distance_cm is not None):
            try:
                sync_thingspeak(latest_distance_cm)
            except Exception as e:
                print(f"ThingSpeak error: {e}")
            last_upload = now

        time.sleep(0.05)

except KeyboardInterrupt:
    vl53.stop_ranging()
    led.value = False
    led.deinit()
    print("\nStopped cleanly.")
