#pragma once

#include "../ir/function.h"
#include "../analysis/cfg/cfg_builder.h"
#include <sstream>
#include <string>

namespace retrograde::decompiler {

using namespace retrograde::ir;

struct PseudoInstruction {
    std::string operation;
    std::vector<std::string> operands;
    std::string comment;
};

struct PseudoBlock {
    std::string label;
    std::vector<PseudoInstruction> instructions;
};

struct PseudoFunction {
    std::string name;
    std::string return_type;
    std::vector<std::string> parameters;
    std::vector<PseudoBlock> blocks;
};

class Decompiler {
public:
    PseudoFunction decompile(FunctionRef ir_function);
    std::string emitPseudoC(const PseudoFunction& func);

private:
    PseudoInstruction translateToPseudo(const IRInstruction& inst);
    std::string valueToString(const core::Value& val);
    std::string opcodeToString(core::IROpcode opcode);
};

}
