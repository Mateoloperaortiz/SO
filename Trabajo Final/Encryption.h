#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <vector>
#include <string>

class Encryption {
public:
    // Vigenère Cipher
    static std::vector<char> EncryptVigenere(const std::vector<char>& data, const std::string& key);
    static std::vector<char> DecryptVigenere(const std::vector<char>& data, const std::string& key);
};

#endif // ENCRYPTION_H
