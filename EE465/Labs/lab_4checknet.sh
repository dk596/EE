hostname -I | awk '{print $1}'
ip -br link show wlan0 | awk '{print $1, $2, $3}'
ip -br link show eth0 | awk '{print $1, $2, $3}'
hostname
whoami
uname -s
awk -F= '/^ID=/ {print $2}' /etc/os-release | tr -d '"'
