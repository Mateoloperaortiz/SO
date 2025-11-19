#include "Compression.h"

std::vector<char> Compression::CompressRLE(const std::vector<char>& data) {
    std::vector<char> compressed;
    if (data.empty()) return compressed;

    size_t n = data.size();
    for (size_t i = 0; i < n; ++i) {
        unsigned char count = 1;
        while (i + 1 < n && data[i] == data[i + 1] && count < 255) {
            i++;
            count++;
        }
        compressed.push_back(data[i]);
        compressed.push_back(static_cast<char>(count));
    }
    return compressed;
}

std::vector<char> Compression::DecompressRLE(const std::vector<char>& data) {
    std::vector<char> decompressed;
    if (data.empty()) return decompressed;

    size_t n = data.size();
    for (size_t i = 0; i < n; i += 2) {
        if (i + 1 >= n) break; // Should not happen if valid RLE
        char val = data[i];
        unsigned char count = static_cast<unsigned char>(data[i + 1]);
        for (int k = 0; k < count; ++k) {
            decompressed.push_back(val);
        }
    }
    return decompressed;
}
