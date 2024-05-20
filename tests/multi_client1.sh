#!/bin/bash
# ./bin/server --directory tests/example_dir/
# This script is used to test the multiple clients accessing scenario.

URL="http://localhost:8080"

source compare.sh

perform_curl() {
    local id="$1"
    curl -s "$URL" -o "out/test${id}.html"
}

num_clients=10
all_success=true

for ((i=1; i<=num_clients; i++)); do
    perform_curl "$i" &
done

wait

for ((i=1; i<=num_clients; i++)); do
    if ! compare_files "sample1.html" "out/test${i}.html"; then
        all_success=false
        break
    fi
    rm -f "out/test${i}.html"
done

if [ "$all_success" = true ]; then
    echo "Success: All files are identical."
else
    echo "Fail: One or more files differ."
fi
