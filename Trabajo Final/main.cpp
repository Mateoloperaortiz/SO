#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "file_manager.h"
#include "compressor.h"
#include "encryptor.h"
#include "thread_manager.h"

struct Options {
    bool compress = false;
    bool decompress = false;
    bool encrypt = false;
    bool decrypt = false;
    std::string compressionAlgorithm = "huffman";
    std::string encryptionAlgorithm = "vigenere";
    std::string inputPath;
    std::string outputPath;
    std::string key;
};

void showHelp() {
    std::cout << "Uso: fileutil [opciones]\n\n";
    std::cout << "Operaciones:\n";
    std::cout << "  -c              Comprimir\n";
    std::cout << "  -d              Descomprimir\n";
    std::cout << "  -e              Encriptar\n";
    std::cout << "  -u              Desencriptar\n";
    std::cout << "  Las operaciones se pueden combinar (ej: -ce para comprimir y encriptar)\n\n";
    std::cout << "Opciones:\n";
    std::cout << "  --comp-alg      Algoritmo de compresión (huffman) [default: huffman]\n";
    std::cout << "  --enc-alg       Algoritmo de encriptación (vigenere) [default: vigenere]\n";
    std::cout << "  -i <ruta>       Ruta de entrada (archivo o directorio)\n";
    std::cout << "  -o <ruta>       Ruta de salida (archivo o directorio)\n";
    std::cout << "  -k <clave>      Clave secreta para encriptación/desencriptación\n";
    std::cout << "  -h, --help      Mostrar esta ayuda\n\n";
    std::cout << "Ejemplos:\n";
    std::cout << "  fileutil -c -i archivo.txt -o archivo.huf\n";
    std::cout << "  fileutil -ce -i directorio/ -o comprimido/ -k mi_clave\n";
    std::cout << "  fileutil -du -i archivo.enc -o archivo.txt -k mi_clave\n";
}

bool parseArguments(int argc, char* argv[], Options& opts) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            showHelp();
            return false;
        }
        else if (arg == "--comp-alg" && i + 1 < argc) {
            opts.compressionAlgorithm = argv[++i];
        }
        else if (arg == "--enc-alg" && i + 1 < argc) {
            opts.encryptionAlgorithm = argv[++i];
        }
        else if (arg == "-i" && i + 1 < argc) {
            opts.inputPath = argv[++i];
        }
        else if (arg == "-o" && i + 1 < argc) {
            opts.outputPath = argv[++i];
        }
        else if (arg == "-k" && i + 1 < argc) {
            opts.key = argv[++i];
        }
        else if (arg.length() > 1 && arg[0] == '-' && arg[1] != '-') {
            for (size_t j = 1; j < arg.length(); j++) {
                char c = arg[j];
                if (c == 'c') {
                    opts.compress = true;
                }
                else if (c == 'd') {
                    opts.decompress = true;
                }
                else if (c == 'e') {
                    opts.encrypt = true;
                }
                else if (c == 'u') {
                    opts.decrypt = true;
                }
            }
        }
    }
    
    if (opts.inputPath.empty() || opts.outputPath.empty()) {
        std::cerr << "Error: Se requieren -i y -o\n";
        showHelp();
        return false;
    }
    
    if (!opts.compress && !opts.decompress && !opts.encrypt && !opts.decrypt) {
        std::cerr << "Error: Se debe especificar al menos una operación (-c, -d, -e, -u)\n";
        showHelp();
        return false;
    }
    
    if ((opts.encrypt || opts.decrypt) && opts.key.empty()) {
        std::cerr << "Error: Se requiere -k para operaciones de encriptación/desencriptación\n";
        return false;
    }
    
    return true;
}

std::string getOutputFileName(const std::string& inputFile, const Options& opts, const std::string& inputBasePath = "") {
    std::string outputFile = opts.outputPath;
    
    bool isOutputDir = FileManager::isDirectory(opts.outputPath) || 
                       opts.outputPath.back() == '\\' || 
                       opts.outputPath.back() == '/';
    
    if (!isOutputDir && !FileManager::isFile(opts.outputPath)) {
        size_t lastDot = opts.outputPath.find_last_of(".");
        size_t lastSlash = opts.outputPath.find_last_of("\\/");
        
        if (lastDot == std::string::npos || (lastSlash != std::string::npos && lastDot < lastSlash)) {
            isOutputDir = true;
            FileManager::createDirectory(opts.outputPath);
        }
    }
    
    if (isOutputDir) {
        if (!outputFile.empty() && outputFile.back() != '\\' && outputFile.back() != '/') {
            outputFile += "\\";
        }
        
        if (!inputBasePath.empty()) {
            std::string relativePath = inputFile;
            std::string normalizedBase = inputBasePath;
            if (normalizedBase.back() != '\\' && normalizedBase.back() != '/') {
                normalizedBase += "\\";
            }
            
            if (relativePath.find(normalizedBase) == 0) {
                relativePath = relativePath.substr(normalizedBase.length());
            } else {
                size_t lastSlash = inputFile.find_last_of("\\/");
                relativePath = (lastSlash == std::string::npos) ? inputFile : inputFile.substr(lastSlash + 1);
            }
            
            outputFile += relativePath;
        } else {
            size_t lastSlash = inputFile.find_last_of("\\/");
            std::string fileName = (lastSlash == std::string::npos) ? inputFile : inputFile.substr(lastSlash + 1);
            outputFile += fileName;
        }
    }
    
    std::string extension;
    if (opts.compress) extension += ".huf";
    if (opts.encrypt) extension += ".enc";
    
    if (opts.decompress) {
        size_t extPos = outputFile.find(".huf");
        if (extPos != std::string::npos) {
            outputFile.erase(extPos, 4);
        }
    }
    if (opts.decrypt) {
        size_t extPos = outputFile.find(".enc");
        if (extPos != std::string::npos) {
            outputFile.erase(extPos, 4);
        }
    }
    
    if (!extension.empty() && outputFile.find(extension) == std::string::npos) {
        outputFile += extension;
    }
    
    return outputFile;
}

bool processFile(const std::string& inputFile, const std::string& outputFile, const Options& opts) {
    std::vector<unsigned char> data;
    
    if (!FileManager::readFile(inputFile, data)) {
        return false;
    }
    
    if (opts.decrypt) {
        Encryptor encryptor(opts.key);
        data = encryptor.decrypt(data);
        if (data.empty()) {
            std::cerr << "Error desencriptando archivo: " << inputFile << std::endl;
            return false;
        }
    }
    
    if (opts.decompress) {
        Compressor compressor;
        data = compressor.decompress(data);
    }
    
    if (opts.compress) {
        Compressor compressor;
        data = compressor.compress(data);
    }
    
    if (opts.encrypt) {
        Encryptor encryptor(opts.key);
        data = encryptor.encrypt(data);
        if (data.empty()) {
            std::cerr << "Error encriptando archivo: " << inputFile << std::endl;
            return false;
        }
    }
    
    return FileManager::writeFile(outputFile, data);
}

int main(int argc, char* argv[]) {
    Options opts;
    
    if (!parseArguments(argc, argv, opts)) {
        return 1;
    }
    
    bool isInputDir = FileManager::isDirectory(opts.inputPath);
    bool isInputFile = FileManager::isFile(opts.inputPath);
    
    if (!isInputDir && !isInputFile) {
        std::cerr << "Error: La ruta de entrada no existe: " << opts.inputPath << std::endl;
        return 1;
    }
    
    if (isInputFile) {
        std::string outputFile = getOutputFileName(opts.inputPath, opts);
        
        if (processFile(opts.inputPath, outputFile, opts)) {
            std::cout << "Archivo procesado exitosamente: " << opts.inputPath << " -> " << outputFile << std::endl;
            return 0;
        } else {
            std::cerr << "Error procesando archivo: " << opts.inputPath << std::endl;
            return 1;
        }
    }
    
    std::vector<std::string> files = FileManager::getAllFiles(opts.inputPath);
    
    if (files.empty()) {
        std::cout << "No se encontraron archivos en el directorio: " << opts.inputPath << std::endl;
        return 0;
    }
    
    std::cout << "Procesando " << files.size() << " archivo(s) de forma concurrente..." << std::endl;
    
    ThreadManager threadManager;
    
    std::string normalizedInputPath = opts.inputPath;
    if (normalizedInputPath.back() != '\\' && normalizedInputPath.back() != '/') {
        normalizedInputPath += "\\";
    }
    auto getOutputPath = [&opts, normalizedInputPath](const std::string& inputFile) -> std::string {
        return getOutputFileName(inputFile, opts, normalizedInputPath);
    };
    
    auto processOperation = [&opts](const std::vector<unsigned char>& input, std::vector<unsigned char>& output) -> bool {
        std::vector<unsigned char> data = input;
        
        if (opts.decrypt) {
            Encryptor encryptor(opts.key);
            data = encryptor.decrypt(data);
            if (data.empty() && !input.empty()) {
                return false;
            }
        }
        
        if (opts.decompress) {
            Compressor compressor;
            data = compressor.decompress(data);
        }
        
        if (opts.compress) {
            Compressor compressor;
            data = compressor.compress(data);
        }
        
        if (opts.encrypt) {
            Encryptor encryptor(opts.key);
            data = encryptor.encrypt(data);
            if (data.empty() && !input.empty()) {
                return false;
            }
        }
        
        output = data;
        return true;
    };
    
    threadManager.processFilesConcurrently(files, opts.outputPath, getOutputPath, processOperation);
    threadManager.waitForCompletion();
    
    std::cout << "\nProcesamiento completado:\n";
    std::cout << "  Archivos procesados: " << threadManager.getProcessedCount() << std::endl;
    std::cout << "  Archivos con error: " << threadManager.getFailedCount() << std::endl;
    
    return threadManager.getFailedCount() == 0 ? 0 : 1;
}