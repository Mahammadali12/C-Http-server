# Send 10 concurrent requests using curl
for i in {1..10}; do
    curl -s http://localhost:8080/index.html >/dev/null &
    pid=$!  # Get PID of last background process
    ls -l /proc/$pid/fd
done

# Wait for all background processes to finish
wait
echo "All requests completed"