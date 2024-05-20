#!/bin/bash

# This script is used to test the single client traversing and downloading.

URL="http://localhost:8080/dir_in_dir"

HTML_FILE="test2.html"

source compare.sh

curl -s "$URL/most_water2.c" -o "out/most_water_downloaded.c"

compare_files "example_dir/dir_in_dir/most_water2.c" "out/most_water_downloaded.c"

if [ $? -eq 0 ]; then
    echo "Success: All files are identical."
else
    echo "Fail: One or more files differ."
fi

rm -f "out/most_water_downloaded.c"
