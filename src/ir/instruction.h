#pragma once

#include "../core/types/types.h"
#include <vector>
#include <string>

namespace retrograde::ir {

using namespace retrograde::core;

struct IRInstruction {
    uint64_t id = 0;
    IROpcode opcode = IROpcode::Nop;
    std::vector<Value> operands;
    std::optional<Value> result;
    InstructionAddress address;
    Condition condition = Condition::Always;
    std::string comment;
};

}
