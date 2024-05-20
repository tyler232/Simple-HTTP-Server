#!/bin/bash
# ./bin/server --directory tests/example_dir/
# This script is used to test the single client accessing scenario.

URL="http://localhost:8080"

HTML_FILE="test1.html"

source compare.sh

curl -s "$URL" -o "out/$HTML_FILE"

compare_files "sample1.html" "out/$HTML_FILE"

if [ $? -eq 0 ]; then
    echo "Success: All files are identical."
else
    echo "Fail: One or more files differ."
fi

rm -f "out/$HTML_FILE"

