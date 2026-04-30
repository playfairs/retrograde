#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace retrograde::bin {

enum class SectionType {
    Unknown,
    Code,
    Data,
    ReadOnlyData,
    BSS,
    SymbolTable,
    StringTable,
    Relocation
};

struct Section {
    std::string name;
    SectionType type = SectionType::Unknown;
    uint64_t virtual_address = 0;
    uint64_t file_offset = 0;
    uint64_t size = 0;
    std::vector<uint8_t> data;
    uint32_t flags = 0;

    bool isExecutable() const {
        return type == SectionType::Code;
    }

    bool isReadable() const {
        return type != SectionType::BSS;
    }

    bool isWritable() const {
        return type == SectionType::Data || type == SectionType::BSS;
    }
};

using SectionRef = std::shared_ptr<Section>;

}
