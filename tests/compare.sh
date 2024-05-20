# compare.sh

# Function to compare files
compare_files() {
    local file1="$1"
    local file2="$2"
    if [ -f "$file1" ] && [ -f "$file2" ]; then
        diff_output=$(diff -q "$file1" "$file2")
        if [ $? -eq 0 ]; then
            return 0
        else
            return 1
        fi
    else
        return 1
    fi
}