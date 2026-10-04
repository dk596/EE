def p1():
    while True:
        try:
            # Get input from the user
            user_input = input("Enter a positive number (n): ")
            n = int(user_input)
            
            # Ensure the number is strictly positive
            if n > 0:
                break
            else:
                print("Please enter a number greater than 0.")
        except ValueError:
            print("Invalid input! Please enter a valid whole number.")

    print(f"\nYou entered: {n}")
    # Calculate the sum from 1 to n
    total_sum = sum(range(1, n + 1))
    
    # Print the sum result
    print(f"\nThe sum of numbers from 1-{n} is: {total_sum}")
    
    # Determine if the final sum is even or odd
    if total_sum % 2 == 0:
        print(f"\nThe sum ({total_sum}) is an EVEN number.")
    else:
        print(f"\nThe sum ({total_sum}) is an ODD number.")

if __name__ == "__main__":
    p1()
