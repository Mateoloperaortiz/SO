#include "file_manager.h"
#include <iostream>
#include <algorithm>

bool FileManager::readFile(const std::string& filePath, std::vector<unsigned char>& buffer) {
    HANDLE hFile = CreateFileA(
        filePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: No se pudo abrir el archivo " << filePath << std::endl;
        return false;
    }
    
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        return false;
    }
    
    if (fileSize.QuadPart == 0) {
        buffer.clear();
        CloseHandle(hFile);
        return true;
    }
    
    buffer.resize(static_cast<size_t>(fileSize.QuadPart));
    
    DWORD bytesRead = 0;
    if (!ReadFile(hFile, buffer.data(), static_cast<DWORD>(fileSize.QuadPart), &bytesRead, NULL)) {
        CloseHandle(hFile);
        return false;
    }
    
    CloseHandle(hFile);
    
    if (bytesRead != fileSize.QuadPart) {
        buffer.resize(bytesRead);
    }
    
    return true;
}

bool FileManager::writeFile(const std::string& filePath, const std::vector<unsigned char>& data) {
    size_t lastSlash = filePath.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        std::string dirPath = filePath.substr(0, lastSlash);
        createDirectory(dirPath);
    }
    
    HANDLE hFile = CreateFileA(
        filePath.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: No se pudo crear el archivo " << filePath << std::endl;
        return false;
    }
    
    if (data.empty()) {
        CloseHandle(hFile);
        return true;
    }
    
    DWORD bytesWritten = 0;
    if (!WriteFile(hFile, data.data(), static_cast<DWORD>(data.size()), &bytesWritten, NULL)) {
        CloseHandle(hFile);
        return false;
    }
    
    CloseHandle(hFile);
    
    return bytesWritten == data.size();
}

bool FileManager::isDirectory(const std::string& path) {
    DWORD dwAttrib = GetFileAttributesA(path.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool FileManager::isFile(const std::string& path) {
    DWORD dwAttrib = GetFileAttributesA(path.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
            !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::vector<std::string> FileManager::getAllFiles(const std::string& directory) {
    std::vector<std::string> files;
    
    std::string searchPath = directory;
    if (searchPath.back() != '\\' && searchPath.back() != '/') {
        searchPath += "\\";
    }
    searchPath += "*";
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        return files;
    }
    
    do {
        std::string fileName = findData.cFileName;
        
        if (fileName == "." || fileName == "..") {
            continue;
        }
        
        std::string fullPath = directory;
        if (fullPath.back() != '\\' && fullPath.back() != '/') {
            fullPath += "\\";
        }
        fullPath += fileName;
        
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            std::vector<std::string> subFiles = getAllFiles(fullPath);
            files.insert(files.end(), subFiles.begin(), subFiles.end());
        } else {
            files.push_back(fullPath);
        }
    } while (FindNextFileA(hFind, &findData));
    
    FindClose(hFind);
    return files;
}

bool FileManager::createDirectory(const std::string& path) {
    if (isDirectory(path)) {
        return true;
    }
    
    size_t lastSlash = path.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        std::string parentPath = path.substr(0, lastSlash);
        createDirectory(parentPath);
    }
    
    return CreateDirectoryA(path.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
}

size_t FileManager::getFileSize(const std::string& filePath) {
    HANDLE hFile = CreateFileA(
        filePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        return 0;
    }
    
    CloseHandle(hFile);
    return static_cast<size_t>(fileSize.QuadPart);
}

bool FileManager::deleteFile(const std::string& filePath) {
    return DeleteFileA(filePath.c_str()) != 0;
}