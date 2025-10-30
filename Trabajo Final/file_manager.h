#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
#include <windows.h>

class FileManager {
public:
    static bool readFile(const std::string& filePath, std::vector<unsigned char>& buffer);
    static bool writeFile(const std::string& filePath, const std::vector<unsigned char>& data);
    static bool isDirectory(const std::string& path);
    static bool isFile(const std::string& path);
    static std::vector<std::string> getAllFiles(const std::string& directory);
    static bool createDirectory(const std::string& path);
    static size_t getFileSize(const std::string& filePath);
    static bool deleteFile(const std::string& filePath);
};

#endif