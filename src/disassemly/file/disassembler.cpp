#include "disassembler.h"

namespace retrograde::disasm {

Disassembler::Disassembler(std::unique_ptr<ArchitectureDecoder> decoder)
    : decoder_(std::move(decoder)) {}

std::vector<FunctionDisassembly> Disassembler::disassembleFunctions(
    const bin::Section& section,
    const std::vector<bin::Symbol>& functions) {
    
    std::vector<FunctionDisassembly> results;
    auto all_instructions = decoder_->decodeSection(section);
    
    for (const auto& func : functions) {
        FunctionDisassembly fd;
        fd.symbol = func;
        
        for (const auto& inst : all_instructions) {
            if (inst.address.virtual_address >= func.address &&
                inst.address.virtual_address < func.address + func.size) {
                fd.address_to_index[inst.address.virtual_address] = fd.instructions.size();
                fd.instructions.push_back(inst);
            }
        }
        
        results.push_back(std::move(fd));
    }
    
    return results;
}

std::vector<DecodedInstruction> Disassembler::disassembleSection(const bin::Section& section) {
    return decoder_->decodeSection(section);
}

}
