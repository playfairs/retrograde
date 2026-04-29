#pragma once

#include "basic_block.h"
#include <string>
#include <vector>
#include <map>

namespace retrograde::ir {

struct IRFunction {
    uint64_t id = 0;
    uint64_t entry_address = 0;
    std::string name;
    std::vector<BlockRef> blocks;
    BlockRef entry_block;
    std::map<uint64_t, BlockRef> block_by_address;

    BlockRef createBlock(uint64_t address, std::string label) {
        auto block = std::make_shared<BasicBlock>();
        block->id = blocks.size();
        block->start_address = address;
        block->label = std::move(label);
        if (blocks.empty()) {
            entry_block = block;
        }
        blocks.push_back(block);
        block_by_address[address] = block;
        return block;
    }
};

using FunctionRef = std::shared_ptr<IRFunction>;

}
