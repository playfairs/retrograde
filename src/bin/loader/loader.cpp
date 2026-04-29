#include "loader.h"
#include <fstream>
#include <iostream>

namespace retrograde::bin {

core::Result<BinaryRef> Loader::load(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        return core::Error::make("Cannot open file: " + filepath);
    }

    uint8_t magic[4];
    file.read(reinterpret_cast<char*>(magic), 4);
    file.seekg(0);

    if (magic[0] == 0x7F && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F') {
        return loadElf(filepath);
    }
    
    if (magic[0] == 'M' && magic[1] == 'Z') {
        return loadPe(filepath);
    }
    
    if (magic[0] == 0xFE && magic[1] == 0xED && magic[2] == 0xFA) {
        return loadMachO(filepath);
    }
    if (magic[0] == 0xCF && magic[1] == 0xFA && magic[2] == 0xED && magic[3] == 0xFE) {
        return loadMachO(filepath);
    }

    auto binary = std::make_shared<Binary>();
    binary->filename = filepath;
    binary->arch = core::Architecture::X64;
    binary->config.arch = core::Architecture::X64;
    binary->config.pointer_size = 8;

    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    auto section = std::make_shared<Section>();
    section->name = ".text";
    section->type = SectionType::Code;
    section->virtual_address = 0x1000;
    section->file_offset = 0;
    section->size = static_cast<uint64_t>(size);
    section->data.resize(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(section->data.data()), size);
    
    binary->sections.push_back(section);
    binary->entry_point = 0x1000;

    return binary;
}

core::Result<BinaryRef> Loader::loadElf(const std::string& filepath) {
    auto binary = std::make_shared<Binary>();
    binary->filename = filepath;
    binary->arch = core::Architecture::X64;
    binary->config.arch = core::Architecture::X64;
    binary->config.pointer_size = 8;
    binary->entry_point = 0x1000;

    std::ifstream file(filepath, std::ios::binary);
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    auto section = std::make_shared<Section>();
    section->name = ".text";
    section->type = SectionType::Code;
    section->virtual_address = 0x1000;
    section->size = static_cast<uint64_t>(size);
    section->data.resize(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(section->data.data()), size);
    
    binary->sections.push_back(section);
    
    return binary;
}

core::Result<BinaryRef> Loader::loadPe(const std::string& filepath) {
    return loadElf(filepath);
}

core::Result<BinaryRef> Loader::loadMachO(const std::string& filepath) {
    return loadElf(filepath);
}

}
