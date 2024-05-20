#!/bin/bash

# This script is used to test the single client directory traversal scenario.

URL="http://localhost:8080/dir_in_dir"

HTML_FILE="test2.html"

source compare.sh

curl -s "$URL/" -o "out/$HTML_FILE"

compare_files "sample2.html" "out/$HTML_FILE"

if [ $? -eq 0 ]; then
    echo "Success: All files are identical."
else
    echo "Fail: One or more files differ."
fi

rm -f "out/$HTML_FILE"
