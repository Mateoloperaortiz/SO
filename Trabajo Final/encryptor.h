#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <vector>
#include <string>

class Encryptor {
private:
    std::string key;
    
    std::vector<unsigned char> expandKey(size_t dataLength);
    void xorOperation(const std::vector<unsigned char>& data, 
                     const std::vector<unsigned char>& expandedKey,
                     std::vector<unsigned char>& result);

public:
    Encryptor(const std::string& key);
    
    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data);
    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& encryptedData);
    void setKey(const std::string& newKey);
};

#endif