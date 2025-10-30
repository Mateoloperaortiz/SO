#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>

class ThreadManager {
private:
    std::vector<std::thread> threads;
    std::mutex outputMutex;
    std::atomic<int> processedFiles;
    std::atomic<int> failedFiles;
    
    void processFile(const std::string& inputPath, 
                    const std::string& outputPath,
                    std::function<bool(const std::vector<unsigned char>&, std::vector<unsigned char>&)> operation);

public:
    ThreadManager();
    ~ThreadManager();
    
    void processFilesConcurrently(
        const std::vector<std::string>& inputFiles,
        const std::string& outputBasePath,
        std::function<std::string(const std::string&)> getOutputPath,
        std::function<bool(const std::vector<unsigned char>&, std::vector<unsigned char>&)> operation
    );
    
    void waitForCompletion();
    
    int getProcessedCount() const { return processedFiles.load(); }
    int getFailedCount() const { return failedFiles.load(); }
    
    void reset();
};

#endif