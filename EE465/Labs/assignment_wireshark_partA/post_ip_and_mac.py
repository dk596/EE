#!/usr/bin/env python3
import subprocess

# 1. Extract the IP Address using Linux tools
try:
    myIPAddress = subprocess.check_output("hostname -I | awk '{print $1}'", shell=True).decode().strip()
except Exception:
    myIPAddress = ""

# 2. Extract the MAC Address using Linux tools
try:
    cmd_mac = "cat /sys/class/net/$(ip route show | awk '/default/ {print $5}')/address"
    myMACAddress = subprocess.check_output(cmd_mac, shell=True).decode().strip()
except Exception:
    myMACAddress = ""

# 3. If an IP address exists, execute the CURL command silently
if myIPAddress:
    url = "https://faridfarahmand.net/EE465/checkipaddress.php"
    
    curl_args = [
        "curl",
        "-X", "POST",
        url,
        "-d", f"IpAddress={myIPAddress}",
        "-d", f"MACAddress={myMACAddress}"
    ]
    
    try:
        # Runs curl and silently discards the server's response
        subprocess.run(
            curl_args,
            stdout=subprocess.DEVNULL,  # Discards website output text
            stderr=subprocess.DEVNULL,  # Discards curl progress meter text
            check=True
        )
        print("Data successfully sent via curl.")
        
    except subprocess.CalledProcessError:
        print("CURL command failed to execute.")
