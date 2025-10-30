#include "compressor.h"
#include <iostream>
#include <bitset>

Compressor::Compressor() : root(nullptr) {
}

Compressor::~Compressor() {
    deleteTree(root);
}

std::map<unsigned char, int> Compressor::buildFrequencyTable(const std::vector<unsigned char>& data) {
    std::map<unsigned char, int> frequencies;
    for (unsigned char byte : data) {
        frequencies[byte]++;
    }
    return frequencies;
}

Compressor::HuffmanNode* Compressor::buildHuffmanTree(const std::map<unsigned char, int>& frequencies) {
    if (frequencies.empty()) {
        return nullptr;
    }
    
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> pq;
    
    for (const auto& pair : frequencies) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();
        
        HuffmanNode* combined = new HuffmanNode(left, right);
        pq.push(combined);
    }
    
    return pq.top();
}

void Compressor::generateCodes(HuffmanNode* node, const std::string& code) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        huffmanCodes[node->byte] = code.empty() ? "0" : code;
    } else {
        generateCodes(node->left, code + "0");
        generateCodes(node->right, code + "1");
    }
}

void Compressor::deleteTree(HuffmanNode* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

void Compressor::serializeTree(HuffmanNode* node, std::vector<unsigned char>& output) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        output.push_back(1);
        output.push_back(node->byte);
    } else {
        output.push_back(0);
        serializeTree(node->left, output);
        serializeTree(node->right, output);
    }
}

Compressor::HuffmanNode* Compressor::deserializeTree(const std::vector<unsigned char>& data, size_t& pos) {
    if (pos >= data.size()) return nullptr;
    
    if (data[pos] == 1) {
        pos++;
        if (pos >= data.size()) return nullptr;
        return new HuffmanNode(data[pos++], 0);
    } else {
        pos++;
        HuffmanNode* left = deserializeTree(data, pos);
        HuffmanNode* right = deserializeTree(data, pos);
        return new HuffmanNode(left, right);
    }
}

std::vector<unsigned char> Compressor::bitsToBytes(const std::string& bits) {
    std::vector<unsigned char> bytes;
    size_t bitCount = bits.size();
    
    for (size_t i = 0; i < bitCount; i += 8) {
        unsigned char byte = 0;
        for (size_t j = 0; j < 8 && (i + j) < bitCount; j++) {
            if (bits[i + j] == '1') {
                byte |= (1 << (7 - j));
            }
        }
        bytes.push_back(byte);
    }
    
    return bytes;
}

std::string Compressor::bytesToBits(const std::vector<unsigned char>& bytes, size_t bitCount) {
    std::string bits;
    bits.reserve(bitCount);
    
    for (size_t i = 0; i < bytes.size() && bits.size() < bitCount; i++) {
        for (int j = 7; j >= 0 && bits.size() < bitCount; j--) {
            bits += ((bytes[i] >> j) & 1) ? '1' : '0';
        }
    }
    
    return bits;
}

std::vector<unsigned char> Compressor::compress(const std::vector<unsigned char>& data) {
    if (data.empty()) {
        return std::vector<unsigned char>({0, 0, 0, 0});
    }
    
    deleteTree(root);
    huffmanCodes.clear();
    
    std::map<unsigned char, int> frequencies = buildFrequencyTable(data);
    
    root = buildHuffmanTree(frequencies);
    if (!root) {
        return std::vector<unsigned char>();
    }
    
    generateCodes(root, "");
    
    std::string encodedBits;
    for (unsigned char byte : data) {
        encodedBits += huffmanCodes[byte];
    }
    
    std::vector<unsigned char> compressed;
    std::vector<unsigned char> treeData;
    serializeTree(root, treeData);
    
    uint32_t treeSize = static_cast<uint32_t>(treeData.size());
    uint32_t bitCount = static_cast<uint32_t>(encodedBits.size());
    
    compressed.insert(compressed.end(), reinterpret_cast<unsigned char*>(&treeSize), 
                     reinterpret_cast<unsigned char*>(&treeSize) + 4);
    compressed.insert(compressed.end(), reinterpret_cast<unsigned char*>(&bitCount), 
                     reinterpret_cast<unsigned char*>(&bitCount) + 4);
    
    compressed.insert(compressed.end(), treeData.begin(), treeData.end());
    
    std::vector<unsigned char> encodedBytes = bitsToBytes(encodedBits);
    compressed.insert(compressed.end(), encodedBytes.begin(), encodedBytes.end());
    
    return compressed;
}

std::vector<unsigned char> Compressor::decompress(const std::vector<unsigned char>& compressedData) {
    if (compressedData.size() < 8) {
        return std::vector<unsigned char>();
    }
    
    uint32_t treeSize = *reinterpret_cast<const uint32_t*>(compressedData.data());
    uint32_t bitCount = *reinterpret_cast<const uint32_t*>(compressedData.data() + 4);
    
    if (treeSize == 0 && bitCount == 0) {
        return std::vector<unsigned char>();
    }
    
    if (compressedData.size() < 8 + treeSize) {
        return std::vector<unsigned char>();
    }
    
    size_t pos = 8;
    deleteTree(root);
    root = deserializeTree(compressedData, pos);
    
    if (!root) {
        return std::vector<unsigned char>();
    }
    
    std::vector<unsigned char> encodedBytes(compressedData.begin() + pos, compressedData.end());
    std::string encodedBits = bytesToBits(encodedBytes, bitCount);
    
    if (encodedBits.empty() || bitCount == 0) {
        return std::vector<unsigned char>();
    }
    
    std::vector<unsigned char> decompressed;
    HuffmanNode* current = root;
    
    for (char bit : encodedBits) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        
        if (!current->left && !current->right) {
            decompressed.push_back(current->byte);
            current = root;
        }
    }
    
    return decompressed;
}