#pragma once

#include <cstdint>
#include <string>

namespace retrograde::core {

enum class Architecture {
    Unknown,
    X86,
    X64,
    ARM32,
    ARM64,
    RISCV32,
    RISCV64
};

struct Config {
    Architecture arch = Architecture::Unknown;
    bool little_endian = true;
    uint32_t pointer_size = 8;
    bool verbose = false;
};

}
