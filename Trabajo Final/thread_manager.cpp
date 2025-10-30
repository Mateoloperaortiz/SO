#include "thread_manager.h"
#include "file_manager.h"
#include <iostream>
#include <algorithm>

ThreadManager::ThreadManager() : processedFiles(0), failedFiles(0) {
}

ThreadManager::~ThreadManager() {
    waitForCompletion();
}

void ThreadManager::processFile(const std::string& inputPath, 
                                const std::string& outputPath,
                                std::function<bool(const std::vector<unsigned char>&, std::vector<unsigned char>&)> operation) {
    try {
        std::vector<unsigned char> inputData;
        if (!FileManager::readFile(inputPath, inputData)) {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cerr << "Error leyendo archivo: " << inputPath << std::endl;
            failedFiles++;
            return;
        }
        
        std::vector<unsigned char> outputData;
        if (!operation(inputData, outputData)) {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cerr << "Error procesando archivo: " << inputPath << std::endl;
            failedFiles++;
            return;
        }
        
        if (!FileManager::writeFile(outputPath, outputData)) {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cerr << "Error escribiendo archivo: " << outputPath << std::endl;
            failedFiles++;
            return;
        }
        
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            processedFiles++;
            std::cout << "Procesado: " << inputPath << " -> " << outputPath << std::endl;
        }
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cerr << "Excepción procesando " << inputPath << ": " << e.what() << std::endl;
        failedFiles++;
    }
}

void ThreadManager::processFilesConcurrently(
    const std::vector<std::string>& inputFiles,
    const std::string& outputBasePath,
    std::function<std::string(const std::string&)> getOutputPath,
    std::function<bool(const std::vector<unsigned char>&, std::vector<unsigned char>&)> operation) {
    
    (void)outputBasePath;
    
    reset();
    
    for (const std::string& inputFile : inputFiles) {
        std::string outputPath = getOutputPath(inputFile);
        
        threads.emplace_back([this, inputFile, outputPath, operation]() {
            processFile(inputFile, outputPath, operation);
        });
    }
}

void ThreadManager::waitForCompletion() {
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threads.clear();
}

void ThreadManager::reset() {
    waitForCompletion();
    processedFiles = 0;
    failedFiles = 0;
}