#pragma once

#include "instruction.h"
#include <vector>
#include <memory>

namespace retrograde::ir {

struct BasicBlock;
using BlockRef = std::shared_ptr<BasicBlock>;

struct BasicBlock : public std::enable_shared_from_this<BasicBlock> {
    uint64_t id = 0;
    uint64_t start_address = 0;
    std::string label;
    std::vector<IRInstruction> instructions;
    std::vector<BlockRef> predecessors;
    std::vector<BlockRef> successors;

    void addInstruction(IRInstruction inst) {
        instructions.push_back(std::move(inst));
    }

    void addSuccessor(BlockRef block) {
        successors.push_back(block);
        block->predecessors.push_back(shared_from_this());
    }
};

}
