#!/bin/bash

# 1. Initialize GPIO 5 as an Output and set it Low (OFF)
pinctrl set 5 op dl
echo "Listening on wlan0 for strict Ping triggers..."

# 2. Capture only incoming ping request payloads continuously
# sudo tshark : Runs the packet capture tool with admin rights.
# -l          : Line-buffer mode. Flushes data instantly so the LED reacts with zero lag.
# -nni wlan0  : '-nn' disables DNS/Port lookups (saves CPU and network traffic). '-i wlan0' listens only on Wi-Fi.
# -Y "icmp.type == 8" : Filter: Only capture ICMP "Echo Requests" (incoming pings).
# -T fields : Changes tshark's output from the standard readable summary (timestamps, IP addresses, etc.) into a strict data-extraction mode.
# -e data   : Tells the extractor to only grab the data field of the packet.
# 2>/dev/null : Redirects tshark's background status/error messages to the trash so they don't print to the screen.
# while read -r payload; do : Reads each line of the output (each ping payload) and stores it in the variable 'payload'.
sudo tshark -l -nni wlan0 -Y "icmp.type == 8" -T fields -e data 2>/dev/null | while read -r payload; do
    
    # 3. Exact match for "abc" (Hex: 616263)
    if [[ "$payload" == "616263" ]]; then
        echo "Received exact 'abc' -> LED ON"
        pinctrl set 5 op dh
        
    # 4. Exact match for "ab" (Hex: 6162)
    elif [[ "$payload" == "6162" ]]; then
        echo "Received exact 'ab' -> LED OFF"
        pinctrl set 5 op dl
    fi
    
done
