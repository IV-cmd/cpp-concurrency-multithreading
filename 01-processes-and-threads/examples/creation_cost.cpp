#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>
#include <sys/wait.h>
#include <unistd.h>

using Clock = std::chrono::steady_clock;

double elapsedUs(Clock::time_point start, Clock::time_point end) {
    return std::chrono::duration<double, std::micro>(end - start).count();
}

int main(int argc, char* argv[]) {
    const int count = argc > 1 ? std::atoi(argv[1]) : 1000;
    if (count <= 0) {
        std::cerr << "N must be positive\n";
        return 1;
    }

    const auto threadStart = Clock::now();
    for (int i = 0; i < count; ++i) {
        std::thread([] {}).join();
    }
    const double threadUs = elapsedUs(threadStart, Clock::now());

    const auto processStart = Clock::now();
    for (int i = 0; i < count; ++i) {
        const pid_t child = fork();
        if (child == -1) {
            std::perror("fork");
            return 1;
        }
        if (child == 0) {
            _exit(0);
        }
        int status = 0;
        if (waitpid(child, &status, 0) == -1) {
            std::perror("waitpid");
            return 1;
        }
    }
    const double processUs = elapsedUs(processStart, Clock::now());

    std::cout << std::fixed << std::setprecision(2)
              << "N = " << count << '\n'
              << "threads: " << threadUs / 1000.0 << " ms, "
              << threadUs / count << " us/thread\n"
              << "processes: " << processUs / 1000.0 << " ms, "
              << processUs / count << " us/process\n"
              << "process/thread ratio: " << processUs / threadUs << '\n';
}
