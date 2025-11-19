#include "Concurrency.h"
#include <iostream>

HANDLE Concurrency::RunTask(ThreadFunc func, LPVOID param) {
    DWORD threadId;
    HANDLE hThread = CreateThread(
        NULL,                   // Default security attributes
        0,                      // Default stack size
        func,                   // Thread function
        param,                  // Argument to thread function
        0,                      // Default creation flags
        &threadId               // Returns the thread identifier
    );

    if (hThread == NULL) {
        std::cerr << "Error creating thread. Error: " << GetLastError() << std::endl;
    }

    return hThread;
}

void Concurrency::WaitForAll(const std::vector<HANDLE>& threads) {
    if (threads.empty()) return;

    // WaitForMultipleObjects can wait for at most MAXIMUM_WAIT_OBJECTS (64)
    // If we have more, we need to loop.
    const DWORD maxWait = MAXIMUM_WAIT_OBJECTS;
    size_t count = threads.size();
    size_t processed = 0;

    while (processed < count) {
        DWORD batchSize = (count - processed > maxWait) ? maxWait : static_cast<DWORD>(count - processed);
        const HANDLE* pHandles = &threads[processed];

        DWORD result = WaitForMultipleObjects(batchSize, pHandles, TRUE, INFINITE);
        if (result == WAIT_FAILED) {
            std::cerr << "WaitForMultipleObjects failed. Error: " << GetLastError() << std::endl;
            break;
        }

        processed += batchSize;
    }

    // Close handles
    for (HANDLE h : threads) {
        CloseHandle(h);
    }
}
