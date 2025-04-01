#!/bin/bash -v

# Set the number of iterations
iterations=500

# Set delay between each execution (in seconds)
delay=1

# Loop to execute the command
for ((i=1; i<=$iterations; i++)); do
    echo "Iteration $i of $iterations"
    
    # Replace this line with your actual command
    curl localhost:8080/index.html
    echo "curl localhost:8080/index.html"  # Example command

    
    # sleep $delay  # Remove or set delay=0 for no pause
done

echo "Loop completed!"