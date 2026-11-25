#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

int counter = 0;

void printThreadDetails() {
    counter = 100;
    std::cout << "thread pid " << getpid() << " tid " << ::syscall(SYS_gettid) << '\n';

    std::vector<std::string> tids;
    std::error_code error;
    const std::filesystem::path taskDirectory("/proc/self/task");
    for (std::filesystem::directory_iterator it(taskDirectory, error), end;
         it != end && !error;
         it.increment(error)) {
        tids.push_back(it->path().filename().string());
    }
    if (error) {
        std::cout << "task list unavailable: " << error.message() << '\n';
    } else {
        std::sort(tids.begin(), tids.end());
        std::cout << "/proc/self/task:";
        for (const auto& tid : tids) {
            std::cout << ' ' << tid;
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "forked process\n" << std::flush;
    const pid_t child = fork();
    if (child == -1) {
        std::perror("fork");
        return 1;
    }
    if (child == 0) {
        counter = 100;
        std::cout << "child pid " << getpid() << " &counter "
                  << static_cast<const void*>(&counter) << " value " << counter << '\n' << std::flush;
        _exit(0);
    }

    int status = 0;
    if (waitpid(child, &status, 0) == -1) {
        std::perror("waitpid");
        return 1;
    }
    std::cout << "parent pid " << getpid() << " &counter "
              << static_cast<const void*>(&counter) << " value " << counter << '\n';

    std::cout << "thread in this process\n";
    std::thread worker(printThreadDetails);
    worker.join();
    std::cout << "main pid " << getpid() << " tid " << ::syscall(SYS_gettid)
              << " counter " << counter << '\n';
}
