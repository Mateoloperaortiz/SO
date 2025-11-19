#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "FileManager.h"
#include "Concurrency.h"
#include "Compression.h"
#include "Encryption.h"

struct Config {
    bool compress = false;
    bool decompress = false;
    bool encrypt = false;
    bool decrypt = false;
    std::string compAlg;
    std::string encAlg;
    std::string inputPath;
    std::string outputPath;
    std::string key;
};

struct ThreadData {
    std::string filePath;
    Config config;
};

DWORD WINAPI ProcessFile(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    std::string inputPath = data->filePath;
    Config config = data->config;

    std::cout << "Processing: " << inputPath << std::endl;

    std::vector<char> buffer;
    if (!FileManager::ReadFileContent(inputPath, buffer)) {
        delete data;
        return 1;
    }

    // Order of operations:
    // Compress -> Encrypt
    // Decrypt -> Decompress

    if (config.compress) {
        // Only RLE supported for now
        buffer = Compression::CompressRLE(buffer);
    }

    if (config.encrypt) {
        buffer = Encryption::EncryptVigenere(buffer, config.key);
    }

    if (config.decrypt) {
        buffer = Encryption::DecryptVigenere(buffer, config.key);
    }

    if (config.decompress) {
        buffer = Compression::DecompressRLE(buffer);
    }

    // Construct output path
    // If output is a directory, append filename. If file, use as is (only for single file input).
    // For simplicity, let's assume -o specifies an output directory if input is a directory, 
    // or a full path if input is a file.
    
    std::string outPath;
    if (FileManager::IsDirectory(config.outputPath)) {
         // It's a directory, append filename + suffix
         std::string suffix = "";
         if (config.compress) suffix += ".rle";
         if (config.encrypt) suffix += ".enc";
         // If decrypting/decompressing, maybe remove suffix? 
         // For this simple implementation, let's just append ".out" if not specified.
         if (config.decompress || config.decrypt) suffix += ".dec";
         
         outPath = FileManager::CreateOutputPath(inputPath, config.outputPath, suffix);
    } else {
        // It's a file path
        outPath = config.outputPath;
    }

    if (!FileManager::WriteFileContent(outPath, buffer)) {
        delete data;
        return 1;
    }

    std::cout << "Finished: " << outPath << std::endl;
    delete data;
    return 0;
}

void PrintUsage() {
    std::cout << "Usage: program -[c|d|e|u] -i <input> -o <output> [-k <key>] [--comp-alg <alg>] [--enc-alg <alg>]" << std::endl;
}

int main(int argc, char* argv[]) {
    Config config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.size() > 1 && arg[0] == '-' && arg[1] != '-') {
            // Short options (e.g. -c, -ce, -ud)
            for (size_t j = 1; j < arg.size(); ++j) {
                char c = arg[j];
                if (c == 'c') config.compress = true;
                else if (c == 'd') config.decompress = true;
                else if (c == 'e') config.encrypt = true;
                else if (c == 'u') config.decrypt = true;
                else if (c == 'i') {
                    if (i + 1 < argc) config.inputPath = argv[++i];
                    break; // Stop parsing this arg string
                }
                else if (c == 'o') {
                    if (i + 1 < argc) config.outputPath = argv[++i];
                    break;
                }
                else if (c == 'k') {
                    if (i + 1 < argc) config.key = argv[++i];
                    break;
                }
                // Note: -i, -o, -k usually take next arg, but if they are combined like -io, it's ambiguous. 
                // Standard behavior is usually not to combine taking-arg flags with others in a way that hides the arg.
                // But for -ce, -ud it works.
                // My previous parser handled -i, -o, -k as separate tokens. 
                // Let's keep -i, -o, -k as separate tokens if possible, or handle them if they are the last char.
                // For safety, let's assume -i, -o, -k are ALWAYS separate flags in the usage example, 
                // but -c, -d, -e, -u can be combined.
            }
            
            // Special handling for flags that expect arguments but were passed separately in previous logic
            // If the loop above didn't handle i, o, k because they were not in the combined string (or we want to support -i value)
            // Actually, the previous logic was:
            // if (arg == "-i") ...
            
            // Let's refine:
            // If it's exactly "-i", "-o", "-k", treat as before.
            // If it's a combined flag like "-ce", parse chars.
            // But wait, "-i" matches the combined logic 'i'.
            
            // To be robust and simple:
            // Iterate chars. If 'c', 'd', 'e', 'u' set flag.
            // If 'i', 'o', 'k', consume next argv.
            
            // But wait, if I have "-ce", loop runs for 'c', then 'e'.
            // If I have "-i", loop runs for 'i'.
        }
        else if (arg == "--comp-alg" && i + 1 < argc) config.compAlg = argv[++i];
        else if (arg == "--enc-alg" && i + 1 < argc) config.encAlg = argv[++i];
    }

    if (config.inputPath.empty() || config.outputPath.empty()) {
        PrintUsage();
        return 1;
    }

    // Validate logic
    if ((config.compress && config.decompress) || (config.encrypt && config.decrypt)) {
        std::cerr << "Invalid combination of operations." << std::endl;
        return 1;
    }

    std::vector<std::string> files;
    if (FileManager::IsDirectory(config.inputPath)) {
        files = FileManager::GetFiles(config.inputPath);
    } else {
        files.push_back(config.inputPath);
    }

    std::vector<HANDLE> threads;
    for (const auto& file : files) {
        ThreadData* data = new ThreadData{file, config};
        HANDLE hThread = Concurrency::RunTask(ProcessFile, data);
        if (hThread) {
            threads.push_back(hThread);
        }
    }

    Concurrency::WaitForAll(threads);

    std::cout << "All tasks completed." << std::endl;
    return 0;
}
