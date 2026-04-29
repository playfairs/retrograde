#pragma once

#include "../../ir/function.h"
#include "../../disassemly/file/disassembler.h"
#include <vector>
#include <map>
#include <set>

namespace retrograde::analysis {

using namespace retrograde::ir;
using namespace retrograde::disasm;

class CFGBuilder {
public:
    FunctionRef buildCFG(const FunctionDisassembly& func_disasm);
    
private:
    std::set<uint64_t> findBlockBoundaries(const FunctionDisassembly& func_disasm);
    std::map<uint64_t, BlockRef> createBlocks(
        const FunctionDisassembly& func_disasm,
        const std::set<uint64_t>& boundaries
    );
    void linkBlocks(
        FunctionRef function,
        const FunctionDisassembly& func_disasm,
        const std::map<uint64_t, BlockRef>& blocks
    );
    IROpcode translateOpcode(const DecodedInstruction& inst);
    IRInstruction translateInstruction(const DecodedInstruction& inst, uint64_t id);
};

}
