#include "FileManager.h"
#include <stdexcept>

bool FileManager::IsDirectory(const std::string& path) {
    DWORD attributes = GetFileAttributesA(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false; // Or throw error
    }
    return (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::vector<std::string> FileManager::GetFiles(const std::string& directory) {
    std::vector<std::string> files;
    std::string searchPath = directory + "\\*";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return files;
    }

    do {
        const std::string fileName = findData.cFileName;
        if (fileName != "." && fileName != "..") {
            std::string fullPath = directory + "\\" + fileName;
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Recursively get files if needed, or just list current level
                // For this project, let's recurse to handle full directories
                std::vector<std::string> subFiles = GetFiles(fullPath);
                files.insert(files.end(), subFiles.begin(), subFiles.end());
            } else {
                files.push_back(fullPath);
            }
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);
    return files;
}

bool FileManager::ReadFileContent(const std::string& path, std::vector<char>& buffer) {
    HANDLE hFile = CreateFileA(
        path.c_str(),           // FileName
        GENERIC_READ,           // DesiredAccess
        FILE_SHARE_READ,        // ShareMode
        NULL,                   // SecurityAttributes
        OPEN_EXISTING,          // CreationDisposition
        FILE_ATTRIBUTE_NORMAL,  // FlagsAndAttributes
        NULL                    // TemplateFile
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening file for reading: " << path << " Error: " << GetLastError() << std::endl;
        return false;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        std::cerr << "Error getting file size: " << path << std::endl;
        CloseHandle(hFile);
        return false;
    }

    buffer.resize(fileSize);
    DWORD bytesRead;
    if (!ReadFile(hFile, buffer.data(), fileSize, &bytesRead, NULL)) {
        std::cerr << "Error reading file: " << path << std::endl;
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);
    return true;
}

bool FileManager::WriteFileContent(const std::string& path, const std::vector<char>& buffer) {
    // Ensure directory exists (simple check, assuming output dir exists or is created)
    // For robust implementation, we might need to create parent directories.
    
    HANDLE hFile = CreateFileA(
        path.c_str(),
        GENERIC_WRITE,
        0,                      // No sharing
        NULL,
        CREATE_ALWAYS,          // Overwrite if exists
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Error creating file for writing: " << path << " Error: " << GetLastError() << std::endl;
        return false;
    }

    DWORD bytesWritten;
    if (!WriteFile(hFile, buffer.data(), static_cast<DWORD>(buffer.size()), &bytesWritten, NULL)) {
        std::cerr << "Error writing to file: " << path << std::endl;
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);
    return true;
}

std::string FileManager::CreateOutputPath(const std::string& inputPath, const std::string& outputDir, const std::string& suffix) {
    // Simple implementation: extract filename and append to outputDir with suffix
    size_t lastSlash = inputPath.find_last_of("/\\");
    std::string fileName = (lastSlash == std::string::npos) ? inputPath : inputPath.substr(lastSlash + 1);
    
    // If outputDir is a directory, append filename. If it looks like a file path (ends in extension), use it directly?
    // Requirement says -o [ruta_archivo_o_directorio_salida]
    // We'll assume outputDir is a directory for batch processing, or a full path for single file.
    // For simplicity in this helper, let's assume we are constructing a path inside an output directory.
    
    // Check if outputDir has a trailing slash
    std::string out = outputDir;
    if (!out.empty() && out.back() != '\\' && out.back() != '/') {
        out += "\\";
    }
    
    return out + fileName + suffix;
}
