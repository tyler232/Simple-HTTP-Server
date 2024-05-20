#!/bin/bash
# ./bin/server --directory tests/example_dir/
# This script is used to test downloading huge file

URL="http://localhost:8080/dir_in_dir"

source compare.sh

perform_curl() {
    local id="$1"
    curl -s "$URL/huge.bin" -o "out/huge_download${id}.bin"
}

num_clients=128
all_success=true

for ((i=1; i<=num_clients; i++)); do
    perform_curl "$i" &
done

wait

for ((i=1; i<=num_clients; i++)); do
    if ! compare_files "example_dir/dir_in_dir/huge.bin" "out/huge_download${i}.bin"; then
        all_success=false
        break
    fi
    rm -f "out/huge_download${i}.bin"
done

if [ "$all_success" = true ]; then
    echo "Success: All files are identical."
else
    echo "Fail: One or more files differ."
fi
