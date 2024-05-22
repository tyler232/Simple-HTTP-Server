#!/bin/bash

scripts=("single_client1.sh" "single_client2.sh" "single_client3.sh" "single_client4.sh" "multi_client1.sh" "multi_client2.sh" "multi_client3.sh" "multi_client4.sh")

for script in "${scripts[@]}"; do
    chmod +x "$script"

    echo "Running $script..."
    ./"$script"

    if [ $? -ne 0 ]; then
        echo "$script failed to execute."
        exit 1
    fi
done

echo "All scripts executed successfully."
