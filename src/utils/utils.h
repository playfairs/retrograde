#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace retrograde::utils {

template<typename T>
T readLE(const uint8_t* data) {
    T result = 0;
    for (size_t i = 0; i < sizeof(T); ++i) {
        result |= static_cast<T>(data[i]) << (i * 8);
    }
    return result;
}

template<typename T>
T readBE(const uint8_t* data) {
    T result = 0;
    for (size_t i = 0; i < sizeof(T); ++i) {
        result |= static_cast<T>(data[i]) << ((sizeof(T) - 1 - i) * 8);
    }
    return result;
}

inline std::string toHex(uint64_t value, size_t width = 0) {
    char buf[32];
    if (width > 0) {
        snprintf(buf, sizeof(buf), "0x%0*llx", static_cast<int>(width), static_cast<unsigned long long>(value));
    } else {
        snprintf(buf, sizeof(buf), "0x%llx", static_cast<unsigned long long>(value));
    }
    return std::string(buf);
}

inline std::string bytesToHex(const std::vector<uint8_t>& bytes) {
    std::string result;
    for (auto b : bytes) {
        char buf[4];
        snprintf(buf, sizeof(buf), "%02x", b);
        result += buf;
    }
    return result;
}

}
