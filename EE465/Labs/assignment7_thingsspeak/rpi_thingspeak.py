import time
from collections import deque
import psutil
import requests
from secrets import THINGSPEAK_WRITE_KEY

THINGSPEAK_URL = 'https://api.thingspeak.com/update'
UPDATE_INTERVAL = 20  # Seconds (must be >= 15s for free ThingSpeak accounts)

# Stores the last 20 temperature readings to calculate a rolling average
recent_temps = deque(maxlen=20)

def get_cpu_temp():
    """Reads Raspberry Pi CPU temperature in Celsius."""
    with open('/sys/class/thermal/thermal_zone0/temp', 'r') as f:
        return round(int(f.read().strip()) / 1000.0, 2)

def get_cpu_usage():
    """Measures overall CPU usage percentage over a 1-second window."""
    return psutil.cpu_percent(interval=1)

def send_to_thingspeak(temp_c, cpu_pct, avg_temp_c):
    """Posts temperature (field1), CPU usage (field2), and average temp (field3) to ThingSpeak."""
    payload = {
        'api_key': THINGSPEAK_WRITE_KEY,
        'field1': temp_c,
        'field2': cpu_pct,
        'field3': avg_temp_c
    }
    response = requests.post(THINGSPEAK_URL, data=payload, timeout=10)
    response.raise_for_status()
    
    # ThingSpeak returns the entry ID (a positive integer) on success, or '0' if rate-limited
    entry_id = response.text.strip()
    if entry_id == '0':
        print("Warning: Update rejected by ThingSpeak (sent too quickly, < 15s).")
    else:
        print(f"Uploaded Entry #{entry_id} -> Temp: {temp_c}°C | Avg Temp: {avg_temp_c}°C | CPU: {cpu_pct}%")

def main():
    print("Starting Raspberry Pi telemetry upload to ThingSpeak (Ctrl+C to stop)...")
    while True:
        try:
            temp = get_cpu_temp()
            usage = get_cpu_usage()
            
            recent_temps.append(temp)
            avg_temp = round(sum(recent_temps) / len(recent_temps), 2)
            
            send_to_thingspeak(temp, usage, avg_temp)
        except Exception as e:
            print(f"Error sending data: {e}")

        # Subtract the 1s taken by psutil.cpu_percent(interval=1)
        time.sleep(max(15, UPDATE_INTERVAL - 1))

if __name__ == '__main__':
    main()
