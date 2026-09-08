# ask for name and age
read -rp "Please enter your name: " name
read -rp "Hi, $name! Enter your age: " age

# determine current year
current_year=$(date +%Y)

# determine target year
target_year=$((current_year + (100 - age)))

# output
echo "Hello dear $name!"
echo "You will turn 100 years old in the year $target_year."

