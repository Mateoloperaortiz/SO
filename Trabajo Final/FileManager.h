#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <windows.h>
#include <string>
#include <vector>
#include <iostream>

class FileManager {
public:
    // Check if path is a directory
    static bool IsDirectory(const std::string& path);

    // Get all files in a directory (recursively or flat)
    static std::vector<std::string> GetFiles(const std::string& directory);

    // Read entire file content
    static bool ReadFileContent(const std::string& path, std::vector<char>& buffer);

    // Write content to file
    static bool WriteFileContent(const std::string& path, const std::vector<char>& buffer);

    // Helper to construct output path based on input path and operation
    static std::string CreateOutputPath(const std::string& inputPath, const std::string& outputDir, const std::string& suffix);
};

#endif // FILEMANAGER_H
