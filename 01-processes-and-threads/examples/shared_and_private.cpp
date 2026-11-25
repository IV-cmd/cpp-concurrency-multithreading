#include <array>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <sys/syscall.h>
#include <unistd.h>

int totalRequests = 0;
int* cache = new int(0);
thread_local int requestsOnThisThread = 0;

struct Addresses {
    std::uintptr_t totalRequests;
    std::uintptr_t cache;
    std::uintptr_t clientId;
    std::uintptr_t requestsOnThisThread;
};

std::array<Addresses, 3> addresses{};
const char* names[] = {"main    ", "thread 1", "thread 2"};

void handleRequest(int worker, std::string& report) {
    int clientId = 10;
    ++requestsOnThisThread;

    std::ostringstream output;
    output << names[worker]
           << " tid " << ::syscall(SYS_gettid)
           << " &totalRequests " << static_cast<const void*>(&totalRequests)
           << " cache " << static_cast<const void*>(cache)
           << " &clientId " << static_cast<const void*>(&clientId)
           << " &requestsOnThisThread " << static_cast<const void*>(&requestsOnThisThread);
    report = output.str();

    addresses[worker] = {
        reinterpret_cast<std::uintptr_t>(&totalRequests),
        reinterpret_cast<std::uintptr_t>(cache),
        reinterpret_cast<std::uintptr_t>(&clientId),
        reinterpret_cast<std::uintptr_t>(&requestsOnThisThread),
    };
}

void printSame(const char* label, std::uintptr_t first, std::uintptr_t second) {
    std::cout << label << ": " << (first == second ? "yes" : "no") << '\n';
}

int main() {
    std::array<std::string, 3> reports;
    handleRequest(0, reports[0]);

    std::thread workerA(handleRequest, 1, std::ref(reports[1]));
    std::thread workerB(handleRequest, 2, std::ref(reports[2]));
    workerA.join();
    workerB.join();

    for (const auto& report : reports) {
        std::cout << report << '\n';
    }

    printSame("same &totalRequests", addresses[0].totalRequests, addresses[1].totalRequests);
    printSame("same cache", addresses[0].cache, addresses[1].cache);
    printSame("same &clientId", addresses[0].clientId, addresses[1].clientId);
    printSame("same &requestsOnThisThread",
              addresses[0].requestsOnThisThread, addresses[1].requestsOnThisThread);

    const std::uintptr_t firstClient = addresses[1].clientId;
    const std::uintptr_t secondClient = addresses[2].clientId;
    const std::uintptr_t distance = firstClient > secondClient
        ? firstClient - secondClient
        : secondClient - firstClient;
    std::cout << std::fixed << std::setprecision(1)
              << "thread 1 to thread 2 clientId distance: " << distance / 1024.0 << " KiB ("
              << distance / (1024.0 * 1024.0) << " MiB)\n";

    delete cache;
}
