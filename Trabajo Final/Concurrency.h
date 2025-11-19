#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include <windows.h>
#include <vector>

class Concurrency {
public:
    // Type for the function to be executed in a thread
    typedef DWORD (WINAPI *ThreadFunc)(LPVOID lpParam);

    // Run a task in a new thread
    // Returns handle to the thread
    static HANDLE RunTask(ThreadFunc func, LPVOID param);

    // Wait for all threads to complete
    static void WaitForAll(const std::vector<HANDLE>& threads);
};

#endif // CONCURRENCY_H
