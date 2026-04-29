#pragma once

#include <cstdint>
#include <string>

namespace retrograde::bin {

enum class SymbolType {
    Unknown,
    Function,
    Data,
    Section,
    File,
    Object
};

enum class SymbolBinding {
    Local,
    Global,
    Weak
};

struct Symbol {
    std::string name;
    uint64_t address = 0;
    uint64_t size = 0;
    SymbolType type = SymbolType::Unknown;
    SymbolBinding binding = SymbolBinding::Local;
    uint16_t section_index = 0;

    bool isFunction() const {
        return type == SymbolType::Function;
    }
};

}
