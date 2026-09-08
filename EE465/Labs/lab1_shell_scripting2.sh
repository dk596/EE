score=0
quiz_date=$(date +"%B %d, %Y at %I:%M %p %Z")


# question 1
read -rp "1. What continent contains the United States?" answer1
if [[ "${answer1,,}" == "north america" || "${answer1,,}" == "na" ]]; then
    echo "Correct!"
    ((score++))
else
    echo "Incorrect. The correct answer is North America."
fi

# question 2
read -rp "2. Who is the president of the United States?" answer2
if [[ "${answer2,,}" == "trump" || "${answer2,,}" == "donald trump" ]]; then
    echo "Correct!"
    ((score++))
else
    echo "Incorrect. The correct answer is Trump."
fi

# question 3
read -rp "3. Is Canada part of the United States?" answer3
if [[ "${answer3,,}" == "no" || "${answer3,,}" == "not yet" ]]; then
    echo "Correct!"
    ((score++))
elif [[ "${answer3,,}" == "yes" ]]; then
    echo "You wish."
else
    echo "Incorrect. The correct answer is no."
fi

# output score
echo "Thank you for taking the quiz!"
echo "Date Taken: $quiz_date"
echo "Your Final Score: $score / 3"
