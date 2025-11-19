#include "Encryption.h"

std::vector<char> Encryption::EncryptVigenere(const std::vector<char>& data, const std::string& key) {
    if (key.empty()) return data;
    std::vector<char> encrypted = data;
    size_t keyLen = key.length();
    
    for (size_t i = 0; i < data.size(); ++i) {
        // Simple addition modulo 256
        encrypted[i] = static_cast<char>(data[i] + key[i % keyLen]);
    }
    return encrypted;
}

std::vector<char> Encryption::DecryptVigenere(const std::vector<char>& data, const std::string& key) {
    if (key.empty()) return data;
    std::vector<char> decrypted = data;
    size_t keyLen = key.length();

    for (size_t i = 0; i < data.size(); ++i) {
        // Simple subtraction modulo 256
        decrypted[i] = static_cast<char>(data[i] - key[i % keyLen]);
    }
    return decrypted;
}
