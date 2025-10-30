#include "encryptor.h"
#include <algorithm>
#include <cstring>

Encryptor::Encryptor(const std::string& key) : key(key) {
    if (key.empty()) {
        this->key = "default_key_12345";
    }
}

std::vector<unsigned char> Encryptor::expandKey(size_t dataLength) {
    std::vector<unsigned char> expandedKey;
    expandedKey.reserve(dataLength);
    
    size_t keyLength = key.length();
    if (keyLength == 0) {
        return expandedKey;
    }
    
    for (size_t i = 0; i < dataLength; i++) {
        expandedKey.push_back(static_cast<unsigned char>(key[i % keyLength]));
    }
    
    return expandedKey;
}

void Encryptor::xorOperation(const std::vector<unsigned char>& data, 
                            const std::vector<unsigned char>& expandedKey,
                            std::vector<unsigned char>& result) {
    result.clear();
    result.reserve(data.size());
    
    for (size_t i = 0; i < data.size(); i++) {
        unsigned char dataByte = data[i];
        unsigned char keyByte = expandedKey[i];
        
        unsigned char encrypted = dataByte ^ keyByte;
        encrypted = (encrypted + keyByte) % 256;
        
        result.push_back(encrypted);
    }
}

std::vector<unsigned char> Encryptor::encrypt(const std::vector<unsigned char>& data) {
    if (data.empty()) {
        return std::vector<unsigned char>();
    }
    
    std::vector<unsigned char> expandedKey = expandKey(data.size());
    std::vector<unsigned char> encrypted;
    
    xorOperation(data, expandedKey, encrypted);
    
    return encrypted;
}

std::vector<unsigned char> Encryptor::decrypt(const std::vector<unsigned char>& encryptedData) {
    if (encryptedData.empty()) {
        return std::vector<unsigned char>();
    }
    
    std::vector<unsigned char> expandedKey = expandKey(encryptedData.size());
    std::vector<unsigned char> decrypted;
    decrypted.reserve(encryptedData.size());
    
    for (size_t i = 0; i < encryptedData.size(); i++) {
        unsigned char encryptedByte = encryptedData[i];
        unsigned char keyByte = expandedKey[i];
        
        unsigned char temp = (encryptedByte + 256 - keyByte) % 256;
        
        unsigned char decryptedByte = temp ^ keyByte;
        
        decrypted.push_back(decryptedByte);
    }
    
    return decrypted;
}

void Encryptor::setKey(const std::string& newKey) {
    key = newKey.empty() ? "default_key_12345" : newKey;
}