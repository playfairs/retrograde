#pragma once

#include "../../core/types/types.h"
#include "../../core/config/config.h"
#include "../../bin/sections/section.h"
#include <vector>
#include <string>
#include <cstdint>

namespace retrograde::disasm {

using namespace retrograde::core;

struct DecodedInstruction {
    InstructionAddress address;
    InstructionBytes bytes;
    std::string mnemonic;
    std::string operands;
    uint8_t length = 0;
    bool is_branch = false;
    bool is_call = false;
    bool is_return = false;
    std::optional<uint64_t> branch_target;
};

class ArchitectureDecoder {
public:
    virtual ~ArchitectureDecoder() = default;
    virtual std::vector<DecodedInstruction> decodeSection(const bin::Section& section) = 0;
    virtual bool isBranch(const DecodedInstruction& inst) const = 0;
    virtual bool isCall(const DecodedInstruction& inst) const = 0;
    virtual bool isReturn(const DecodedInstruction& inst) const = 0;
};

class X86Decoder : public ArchitectureDecoder {
public:
    std::vector<DecodedInstruction> decodeSection(const bin::Section& section) override;
    bool isBranch(const DecodedInstruction& inst) const override;
    bool isCall(const DecodedInstruction& inst) const override;
    bool isReturn(const DecodedInstruction& inst) const override;
};

class X64Decoder : public X86Decoder {};

class ARM32Decoder : public ArchitectureDecoder {
public:
    std::vector<DecodedInstruction> decodeSection(const bin::Section& section) override;
    bool isBranch(const DecodedInstruction& inst) const override;
    bool isCall(const DecodedInstruction& inst) const override;
    bool isReturn(const DecodedInstruction& inst) const override;
};

class ARM64Decoder : public ArchitectureDecoder {
public:
    std::vector<DecodedInstruction> decodeSection(const bin::Section& section) override;
    bool isBranch(const DecodedInstruction& inst) const override;
    bool isCall(const DecodedInstruction& inst) const override;
    bool isReturn(const DecodedInstruction& inst) const override;
};

std::unique_ptr<ArchitectureDecoder> createDecoder(core::Architecture arch);

}
