#include "arch.h"
#include <sstream>
#include <iomanip>

namespace retrograde::disasm {

std::unique_ptr<ArchitectureDecoder> createDecoder(core::Architecture arch) {
    switch (arch) {
        case core::Architecture::X86:
            return std::make_unique<X86Decoder>();
        case core::Architecture::X64:
            return std::make_unique<X64Decoder>();
        case core::Architecture::ARM32:
            return std::make_unique<ARM32Decoder>();
        case core::Architecture::ARM64:
            return std::make_unique<ARM64Decoder>();
        default:
            return std::make_unique<X64Decoder>();
    }
}

std::vector<DecodedInstruction> X86Decoder::decodeSection(const bin::Section& section) {
    std::vector<DecodedInstruction> instructions;
    uint64_t offset = 0;
    uint64_t base_addr = section.virtual_address;

    while (offset < section.data.size()) {
        DecodedInstruction inst;
        inst.address.virtual_address = base_addr + offset;
        inst.address.raw = offset;
        
        uint8_t byte = section.data[offset];
        uint8_t length = 1;
        
        if (byte == 0x55) {
            inst.mnemonic = "push";
            inst.operands = "rbp";
        } else if (byte == 0x48 && offset + 1 < section.data.size() && section.data[offset + 1] == 0x89) {
            inst.mnemonic = "mov";
            inst.operands = "rbp, rsp";
            length = 3;
        } else if (byte == 0x90) {
            inst.mnemonic = "nop";
        } else if (byte == 0xC3) {
            inst.mnemonic = "ret";
            inst.is_return = true;
        } else if (byte == 0xE8 && offset + 4 < section.data.size()) {
            inst.mnemonic = "call";
            int32_t rel = *reinterpret_cast<const int32_t*>(&section.data[offset + 1]);
            inst.branch_target = base_addr + offset + 5 + rel;
            inst.is_call = true;
            length = 5;
        } else if (byte == 0xEB && offset + 1 < section.data.size()) {
            inst.mnemonic = "jmp";
            int8_t rel = static_cast<int8_t>(section.data[offset + 1]);
            inst.branch_target = base_addr + offset + 2 + rel;
            inst.is_branch = true;
            length = 2;
        } else {
            inst.mnemonic = "db";
            std::stringstream ss;
            ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            inst.operands = ss.str();
        }

        inst.length = length;
        inst.bytes.insert(inst.bytes.end(), section.data.begin() + offset, 
                          section.data.begin() + offset + length);
        
        instructions.push_back(inst);
        offset += length;
    }

    return instructions;
}

bool X86Decoder::isBranch(const DecodedInstruction& inst) const {
    return inst.is_branch;
}

bool X86Decoder::isCall(const DecodedInstruction& inst) const {
    return inst.is_call;
}

bool X86Decoder::isReturn(const DecodedInstruction& inst) const {
    return inst.is_return;
}

std::vector<DecodedInstruction> ARM32Decoder::decodeSection(const bin::Section& section) {
    std::vector<DecodedInstruction> instructions;
    uint64_t offset = 0;
    uint64_t base_addr = section.virtual_address;

    while (offset + 4 <= section.data.size()) {
        DecodedInstruction inst;
        inst.address.virtual_address = base_addr + offset;
        inst.address.raw = offset;
        inst.length = 4;
        inst.bytes.insert(inst.bytes.end(), section.data.begin() + offset,
                          section.data.begin() + offset + 4);
        
        uint32_t word = *reinterpret_cast<const uint32_t*>(&section.data[offset]);
        
        if ((word & 0x0FF00000) == 0x0BD00000) {
            inst.mnemonic = "pop";
            inst.operands = "{pc}";
            inst.is_return = true;
        } else {
            inst.mnemonic = "unknown";
        }
        
        instructions.push_back(inst);
        offset += 4;
    }

    return instructions;
}

bool ARM32Decoder::isBranch(const DecodedInstruction& inst) const {
    return inst.is_branch;
}

bool ARM32Decoder::isCall(const DecodedInstruction& inst) const {
    return inst.is_call;
}

bool ARM32Decoder::isReturn(const DecodedInstruction& inst) const {
    return inst.is_return;
}

std::vector<DecodedInstruction> ARM64Decoder::decodeSection(const bin::Section& section) {
    std::vector<DecodedInstruction> instructions;
    uint64_t offset = 0;
    uint64_t base_addr = section.virtual_address;

    while (offset + 4 <= section.data.size()) {
        DecodedInstruction inst;
        inst.address.virtual_address = base_addr + offset;
        inst.address.raw = offset;
        inst.length = 4;
        inst.bytes.insert(inst.bytes.end(), section.data.begin() + offset,
                          section.data.begin() + offset + 4);
        inst.mnemonic = "unknown";
        instructions.push_back(inst);
        offset += 4;
    }

    return instructions;
}

bool ARM64Decoder::isBranch(const DecodedInstruction& inst) const {
    return inst.is_branch;
}

bool ARM64Decoder::isCall(const DecodedInstruction& inst) const {
    return inst.is_call;
}

bool ARM64Decoder::isReturn(const DecodedInstruction& inst) const {
    return inst.is_return;
}

}
