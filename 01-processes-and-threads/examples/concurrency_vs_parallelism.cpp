#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <thread>

using Clock = std::chrono::steady_clock;

struct WorkResult {
    int affinityError = 0;
    int cpu = -1;
    double finishMs = 0.0;
    std::uint64_t value = 0;
};

std::uint64_t work() {
    std::uint64_t value = 1;
    for (std::uint64_t i = 0; i < 250'000'000; ++i) {
        value = value * 6364136223846793005ULL + i + 1;
    }
    return value;
}

void pinnedWork(int cpu, Clock::time_point start, WorkResult& result) {
    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet);
    CPU_SET(cpu, &cpuSet);
    result.affinityError =
        pthread_setaffinity_np(pthread_self(), sizeof(cpuSet), &cpuSet);
    if (result.affinityError != 0) {
        return;
    }

    result.value = work();
    result.finishMs = std::chrono::duration<double, std::milli>(Clock::now() - start).count();
    result.cpu = sched_getcpu();
}

void runPinnedPair(const char* label, int firstCpu, int secondCpu) {
    const std::array<int, 2> cpus{firstCpu, secondCpu};
    std::array<WorkResult, 2> results{};
    const auto start = Clock::now();

    std::thread first(pinnedWork, cpus[0], start, std::ref(results[0]));
    std::thread second(pinnedWork, cpus[1], start, std::ref(results[1]));
    first.join();
    second.join();

    bool pinned = true;
    for (std::size_t i = 0; i < results.size(); ++i) {
        if (results[i].affinityError != 0) {
            std::cout << label << ": pinning CPU " << cpus[i] << " failed: "
                      << std::strerror(results[i].affinityError) << "; skipped\n";
            pinned = false;
        }
    }
    if (!pinned) {
        return;
    }

    const double totalMs = std::chrono::duration<double, std::milli>(Clock::now() - start).count();
    std::cout << label << " total: " << totalMs << " ms\n";
    for (std::size_t i = 0; i < results.size(); ++i) {
        std::cout << "thread " << i + 1 << " finish " << results[i].finishMs
                  << " ms on CPU " << results[i].cpu
                  << " (result " << results[i].value << ")\n";
    }
}

int main() {
    std::cout << "hardware_concurrency: " << std::thread::hardware_concurrency() << '\n';
    const auto start = Clock::now();
    const std::uint64_t result = work();
    const double aloneMs = std::chrono::duration<double, std::milli>(Clock::now() - start).count();

    std::cout << std::fixed << std::setprecision(2)
              << "work alone: " << aloneMs << " ms (result " << result << ")\n";
    runPinnedPair("two threads on CPU 0", 0, 0);
    runPinnedPair("threads on CPU 0 and CPU 1", 0, 1);
}
