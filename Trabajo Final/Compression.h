#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <vector>

class Compression {
public:
    // Run-Length Encoding
    static std::vector<char> CompressRLE(const std::vector<char>& data);
    static std::vector<char> DecompressRLE(const std::vector<char>& data);
};

#endif // COMPRESSION_H
