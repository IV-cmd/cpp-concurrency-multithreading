#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

using Clock = std::chrono::steady_clock;

struct Request {
    const char* label;
    int waitMs;
};

struct Timing {
    long long startMs = 0;
    long long doneMs = 0;
};

long long elapsedMs(Clock::time_point start) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
}

void handleRequest(const Request& request, Timing& timing, Clock::time_point start) {
    timing.startMs = elapsedMs(start);
    std::this_thread::sleep_for(std::chrono::milliseconds(request.waitMs));
    timing.doneMs = elapsedMs(start);
}

void printTimings(const std::array<Request, 3>& requests,
                  const std::array<Timing, 3>& timings,
                  long long totalMs) {
    for (std::size_t i = 0; i < requests.size(); ++i) {
        std::cout << std::left << std::setw(9) << requests[i].label
                  << " start " << std::right << std::setw(4) << timings[i].startMs
                  << " ms  done " << std::setw(4) << timings[i].doneMs << " ms\n";
    }
    std::cout << "total: " << totalMs << " ms\n";
}

int main() {
    const std::array<Request, 3> requests{{
        {"A read", 300},
        {"B write", 200},
        {"C exists?", 50},
    }};

    std::cout << "sequential requests\n";
    std::array<Timing, 3> sequential{};
    const auto sequentialStart = Clock::now();
    for (std::size_t i = 0; i < requests.size(); ++i) {
        handleRequest(requests[i], sequential[i], sequentialStart);
    }
    printTimings(requests, sequential, elapsedMs(sequentialStart));

    std::cout << "threaded requests\n";
    std::array<Timing, 3> threaded{};
    const auto threadedStart = Clock::now();
    std::array<std::thread, 3> workers;
    for (std::size_t i = 0; i < requests.size(); ++i) {
        workers[i] = std::thread([&, i] {
            handleRequest(requests[i], threaded[i], threadedStart);
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }
    printTimings(requests, threaded, elapsedMs(threadedStart));
}
