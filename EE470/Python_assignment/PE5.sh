#!/usr/bin/env bash

CYAN='\033[0;36m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

cleanup_pins() {
    echo -e "${YELLOW}\nResetting GPIO states...${NC}"
    if command -v pinctrl &> /dev/null; then
        pinctrl set 17 op dl &>/dev/null
        pinctrl set 22 ip pn &>/dev/null
        pinctrl set 27 ip pn &>/dev/null
    elif command -v raspi-gpio &> /dev/null; then
        raspi-gpio set 17 dl &>/dev/null
    fi
}

run_task() {
    local script_name="$1"
    local needs_sudo="$2"
    local full_path="$DIR/$script_name"

    if [[ ! -f "$full_path" ]]; then
        echo -e "${RED}[ERROR] File not found: $script_name${NC}"
        echo "Make sure $script_name exists in $DIR"
        read -rp "Press Enter to return to menu..."
        return
    fi

    clear
    echo -e "${GREEN}====================================================${NC}"
    echo -e "${GREEN} Running: $script_name${NC}"
    echo -e "${YELLOW} Press Ctrl+C at any time to exit back to the menu.${NC}"
    echo -e "${GREEN}====================================================${NC}\n"

    # Trap SIGINT (Ctrl+C) so Bash does not exit when the child is interrupted
    trap 'echo -e "\n${YELLOW}[Caught Ctrl+C - returning to menu]${NC}"' INT

    if [[ "$needs_sudo" == "true" ]]; then
        if [[ $EUID -ne 0 ]]; then
            sudo python3 "$full_path"
        else
            python3 "$full_path"
        fi
    else
        python3 "$full_path"
    fi

    # Reset pins cleanly after Python exits or gets interrupted
    cleanup_pins

    # Clear trap before pausing so user can Ctrl+C at the prompt if desired
    trap - INT

    echo -e "\n${GREEN}[Execution completed]${NC}"
    read -rp "Press Enter to return to menu..."
}

while true; do
    # When sitting at the menu, allow Ctrl+C to cleanly exit the launcher
    trap 'echo -e "\n${CYAN}Exiting launcher. Goodbye!${NC}"; cleanup_pins; exit 0' INT

    clear
    echo -e "${CYAN}======================================================${NC}"
    echo -e "${CYAN}             Raspberry Pi Lab Launcher                ${NC}"
    echo -e "${CYAN}======================================================${NC}"
    echo -e "  ${YELLOW}1)${NC} switch.py  - Push Button Input & Debounce (GPIO 27)"
    echo -e "  ${YELLOW}2)${NC} PE2.py     - Ping Google on Switch Press (GPIO 27)"
    echo -e "  ${YELLOW}3)${NC} PE3.py     - ICMP Ping Listener -> LED Indicator (GPIO 17)"
    echo -e "  ${YELLOW}4)${NC} PE4.py     - Dual Wi-Fi Switches & No-IP Blink (GPIO 27, 22, 17)"
    echo -e "  ${YELLOW}5)${NC} Reset GPIOs (Turn off LED / restore pins)"
    echo -e "  ${YELLOW}q)${NC} Exit"
    echo -e "${CYAN}------------------------------------------------------${NC}"
    read -rp "Select a script to run [1-5, q]: " choice

    case "$choice" in
        1)
            run_task "switch.py" "false"
            ;;
        2)
            run_task "PE2.py" "false"
            ;;
        3)
            run_task "PE3.py" "true"
            ;;
        4)
            run_task "PE4.py" "true"
            ;;
        5)
            cleanup_pins
            echo -e "${GREEN}GPIO 17, 22, and 27 have been reset.${NC}"
            sleep 1.5
            ;;
        q|Q)
            cleanup_pins
            echo -e "${CYAN}Exiting launcher. Goodbye!${NC}"
            exit 0
            ;;
        *)
            echo -e "${RED}Invalid choice. Please select 1-5 or q.${NC}"
            sleep 1.2
            ;;
    esac
done
