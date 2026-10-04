def p2():
    try:
        with open('/etc/resolv.conf', 'r') as file:
            count = 1
            for line in file:
                if line.startswith('nameserver'):
                    dns_ip = line.split()[1]
                    
                    # The first one found is always primary
                    if count == 1:
                        print(f"Primary DNS: {dns_ip}")
                    # The second one found is the backup
                    elif count == 2:
                        print(f"Backup DNS:  {dns_ip}")
                    # Any extra ones are alternative backups
                    else:
                        print(f"Alt Backup DNS: {dns_ip}")
                        
                    count += 1
    except Exception as e:
        print(f"Error reading DNS configuration: {e}")

if __name__ == "__main__":
    p2()
