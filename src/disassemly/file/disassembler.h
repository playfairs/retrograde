#pragma once

#include "../architecture/arch.h"
#include "../../bin/sections/section.h"
#include "../../bin/symbols/symbol.h"
#include <vector>
#include <map>

namespace retrograde::disasm {

struct FunctionDisassembly {
    bin::Symbol symbol;
    std::vector<DecodedInstruction> instructions;
    std::map<uint64_t, size_t> address_to_index;
};

class Disassembler {
public:
    explicit Disassembler(std::unique_ptr<ArchitectureDecoder> decoder);
    
    std::vector<FunctionDisassembly> disassembleFunctions(
        const bin::Section& section,
        const std::vector<bin::Symbol>& functions
    );
    
    std::vector<DecodedInstruction> disassembleSection(const bin::Section& section);

private:
    std::unique_ptr<ArchitectureDecoder> decoder_;
};

}
