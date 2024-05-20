#!/bin/bash

# This script is used to test the single client downloading.

URL="http://localhost:8080"

source compare.sh

curl -s "$URL/most_water.c" -o "out/most_water_downloaded.c"

compare_files "example_dir/most_water.c" "out/most_water_downloaded.c"

if [ $? -eq 0 ]; then
    echo "Success: All files are identical."
else
    echo "Fail: One or more files differ."
fi

rm -f "out/most_water_downloaded.c"
