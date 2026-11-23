import smtplib
import ssl
from email.message import EmailMessage
from secrets import GMAIL_USER, GMAIL_PASS, RECIPIENT_EMAIL

# Configuration
TEMP_THRESHOLD = 40.0  # Alert threshold in Celsius

def get_cpu_temp():
    """Reads the Raspberry Pi CPU temperature in Celsius from the system thermal zone."""
    with open('/sys/class/thermal/thermal_zone0/temp', 'r') as f:
        temp_millidegrees = int(f.read().strip())
    return temp_millidegrees / 1000.0

def send_alert_email(current_temp):
    """Sends a high-temperature alert email via Gmail SMTP_SSL."""
    msg = EmailMessage()
    msg['From'] = GMAIL_USER
    msg['To'] = RECIPIENT_EMAIL
    msg['Subject'] = f'⚠️ Raspberry Pi High CPU Temp Alert: {current_temp:.1f}°C'
    msg.set_content(
        f'Warning: Your Raspberry Pi CPU temperature has reached {current_temp:.1f}°C, '
        f'which exceeds your threshold of {TEMP_THRESHOLD}°C.'
    )

    context = ssl.create_default_context()
    with smtplib.SMTP_SSL('smtp.gmail.com', 465, context=context) as smtpserver:
        smtpserver.login(GMAIL_USER, GMAIL_PASS)
        smtpserver.send_message(msg)
    print(f"Alert email sent to {RECIPIENT_EMAIL}!")

def main():
    print(f"Raspberry Pi CPU Temp Checker (Threshold: {TEMP_THRESHOLD}°C)")
    print("Press [Enter] to check temperature, or type 'q' and press [Enter] to quit.\n")

    while True:
        user_input = input(">> ").strip().lower()
        if user_input == 'q':
            print("Exiting.")
            break

        try:
            temp = get_cpu_temp()
            print(f"Current CPU Temp: {temp:.1f}°C")

            if temp > TEMP_THRESHOLD:
                print(f"Exceeds {TEMP_THRESHOLD}°C! Sending email alert...")
                send_alert_email(temp)
            else:
                print(f"Normal (below {TEMP_THRESHOLD}°C). No email sent.")

        except Exception as e:
            print(f"Error: {e}")

if __name__ == '__main__':
    main()
    
