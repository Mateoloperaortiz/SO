#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <vector>
#include <string>
#include <map>
#include <queue>

class Compressor {
private:
    struct HuffmanNode {
        unsigned char byte;
        int frequency;
        HuffmanNode* left;
        HuffmanNode* right;
        
        HuffmanNode(unsigned char b, int freq) 
            : byte(b), frequency(freq), left(nullptr), right(nullptr) {}
        
        HuffmanNode(HuffmanNode* l, HuffmanNode* r)
            : byte(0), frequency(l->frequency + r->frequency), left(l), right(r) {}
    };
    
    struct CompareNodes {
        bool operator()(HuffmanNode* a, HuffmanNode* b) {
            return a->frequency > b->frequency;
        }
    };
    
    std::map<unsigned char, std::string> huffmanCodes;
    HuffmanNode* root;
    
    std::map<unsigned char, int> buildFrequencyTable(const std::vector<unsigned char>& data);
    HuffmanNode* buildHuffmanTree(const std::map<unsigned char, int>& frequencies);
    void generateCodes(HuffmanNode* node, const std::string& code);
    void deleteTree(HuffmanNode* node);
    void serializeTree(HuffmanNode* node, std::vector<unsigned char>& output);
    HuffmanNode* deserializeTree(const std::vector<unsigned char>& data, size_t& pos);
    std::vector<unsigned char> bitsToBytes(const std::string& bits);
    std::string bytesToBits(const std::vector<unsigned char>& bytes, size_t bitCount);

public:
    Compressor();
    ~Compressor();
    
    std::vector<unsigned char> compress(const std::vector<unsigned char>& data);
    std::vector<unsigned char> decompress(const std::vector<unsigned char>& compressedData);
};

#endif