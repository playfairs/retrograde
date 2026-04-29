#pragma once

#include "../sections/section.h"
#include "../symbols/symbol.h"
#include "../../core/config/config.h"
#include "../../core/error/error.h"
#include <vector>
#include <string>
#include <memory>

namespace retrograde::bin {

struct Binary {
    std::string filename;
    core::Architecture arch = core::Architecture::Unknown;
    uint64_t entry_point = 0;
    std::vector<SectionRef> sections;
    std::vector<Symbol> symbols;
    core::Config config;

    SectionRef getCodeSection() const {
        for (auto& section : sections) {
            if (section->isExecutable()) {
                return section;
            }
        }
        return nullptr;
    }

    std::vector<Symbol> getFunctions() const {
        std::vector<Symbol> funcs;
        for (const auto& sym : symbols) {
            if (sym.isFunction()) {
                funcs.push_back(sym);
            }
        }
        return funcs;
    }
};

using BinaryRef = std::shared_ptr<Binary>;

class Loader {
public:
    core::Result<BinaryRef> load(const std::string& filepath);
    
private:
    core::Result<BinaryRef> loadElf(const std::string& filepath);
    core::Result<BinaryRef> loadPe(const std::string& filepath);
    core::Result<BinaryRef> loadMachO(const std::string& filepath);
};

}
