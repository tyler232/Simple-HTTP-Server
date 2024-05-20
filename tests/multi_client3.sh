#!/bin/bash
# ./bin/server --directory tests/example_dir/
# This script is used to test the multiple clients downloading scenario.

URL="http://localhost:8080"

source compare.sh

perform_curl() {
    local id="$1"
    curl -s "$URL/most_water.c" -o "out/most_water_downloaded${id}.c"
}

num_clients=10
all_success=true

for ((i=1; i<=num_clients; i++)); do
    perform_curl "$i" &
done

wait

for ((i=1; i<=num_clients; i++)); do
    if ! compare_files "example_dir/most_water.c" "out/most_water_downloaded${i}.c"; then
        all_success=false
        break
    fi
    rm -f "out/most_water_downloaded${i}.c"
done

if [ "$all_success" = true ]; then
    echo "Success: All files are identical."
else
    echo "Fail: One or more files differ."
fi
