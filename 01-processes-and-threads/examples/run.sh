#!/usr/bin/env bash
# Builds and runs the processes-and-threads examples.
ulimit -c 0

for example in sequential_vs_threads shared_and_private process_vs_thread; do
    echo "=== $example"
    g++ -std=c++20 -Wall -Wextra -pthread "$example.cpp" -o "$example" && "./$example"
    rm -f "$example"
done

echo "=== creation_cost"
g++ -std=c++20 -O2 -Wall -Wextra -pthread creation_cost.cpp -o creation_cost && ./creation_cost
rm -f creation_cost

echo "=== concurrency_vs_parallelism"
g++ -std=c++20 -O2 -Wall -Wextra -pthread concurrency_vs_parallelism.cpp -o concurrency_vs_parallelism &&
    ./concurrency_vs_parallelism
rm -f concurrency_vs_parallelism
