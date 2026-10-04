import subprocess

def secret_code():
    target_url = "https://faridfarahmand.net/firstpage.html"
    
    try:
        # Run curl via the command line and capture its text output
        result = subprocess.run(
            ["curl", "-s", "-L", target_url], 
            capture_output=True, 
            text=True, 
            check=True
        )
        
        page_content = result.stdout
        
        # 2. Split the page around the phrase "Secret Code:" and take everything after it
        after_secret = page_content.split("Secret Code:")[1]
        
        # 3. Grab the very first word from that remaining text block
        secret_value = after_secret.split()[0]
        
        print(secret_value)
            
    except Exception:
        print("Error: Could not extract the secret code.")

if __name__ == "__main__":
    secret_code()
