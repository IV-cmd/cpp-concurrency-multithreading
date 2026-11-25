# cpp-concurrency-multithreading

Phase 3 of my systems engineering notes: threads, mutexes, locks, condition variables, atomics, memory ordering, race conditions, deadlocks and thread pools in modern C++. Phase 1 is [cpp-language-foundations](https://github.com/IV-cmd/cpp-language-foundations) and Phase 2 is [cpp-memory-management](https://github.com/IV-cmd/cpp-memory-management).

## Curriculum

| Phase | Topic | Covers |
|---|---|---|
| 1 | [C++ Language Foundations](https://github.com/IV-cmd/cpp-language-foundations) | Compilation, types, pointers, references, functions, classes, object lifetimes, constructors, destructors, inheritance, polymorphism |
| 2 | [Memory Management and Modern C++](https://github.com/IV-cmd/cpp-memory-management) | Stack, heap, RAII, smart pointers, ownership, move/copy semantics, Rule of Zero/Five |
| **3** | **Concurrency and Multithreading** (this repo) | Threads, mutexes, locks, condition variables, atomics, memory ordering, race conditions, deadlocks, thread pools |
| 4 | Linux and Operating Systems | Processes, virtual memory, syscalls, file descriptors, filesystems, scheduling, signals, debugging |
| 5 | Storage Systems and Linux I/O | Block devices, page cache, buffered/direct I/O, `pread`/`pwrite`, `io_uring`, `fsync`, RAID, performance |
| 6 | Distributed Systems | Replication, consistency, quorum, consensus, failure detection, recovery, erasure coding, distributed metadata |

## Phase 3 modules

| # | Module |
|---|---|
| 1 | [Processes and threads](01-processes-and-threads/) |

## Diagrams

Every diagram is a small HTML file in a module's `images/src/` folder. `python3 tools/diagrams/render.py` turns them into the PNGs in `images/` (it needs `pip install playwright`).
