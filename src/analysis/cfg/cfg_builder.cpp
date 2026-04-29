#include "cfg_builder.h"

namespace retrograde::analysis {

FunctionRef CFGBuilder::buildCFG(const FunctionDisassembly& func_disasm) {
    auto function = std::make_shared<IRFunction>();
    function->id = 0;
    function->entry_address = func_disasm.symbol.address;
    function->name = func_disasm.symbol.name.empty() ? "sub_" + std::to_string(func_disasm.symbol.address) : func_disasm.symbol.name;
    
    auto boundaries = findBlockBoundaries(func_disasm);
    auto blocks = createBlocks(func_disasm, boundaries);
    linkBlocks(function, func_disasm, blocks);
    
    return function;
}

std::set<uint64_t> CFGBuilder::findBlockBoundaries(const FunctionDisassembly& func_disasm) {
    std::set<uint64_t> boundaries;
    
    if (!func_disasm.instructions.empty()) {
        boundaries.insert(func_disasm.instructions.front().address.virtual_address);
    }
    
    for (size_t i = 0; i < func_disasm.instructions.size(); ++i) {
        const auto& inst = func_disasm.instructions[i];
        
        if (inst.is_branch || inst.is_call || inst.is_return) {
            uint64_t next_addr = inst.address.virtual_address + inst.length;
            if (i + 1 < func_disasm.instructions.size()) {
                boundaries.insert(next_addr);
            }
        }
        
        if (inst.branch_target.has_value()) {
            boundaries.insert(inst.branch_target.value());
        }
    }
    
    return boundaries;
}

std::map<uint64_t, BlockRef> CFGBuilder::createBlocks(
    const FunctionDisassembly& func_disasm,
    const std::set<uint64_t>& boundaries) {
    
    std::map<uint64_t, BlockRef> blocks;
    uint64_t current_block_addr = 0;
    BlockRef current_block;
    uint64_t inst_id = 0;
    
    for (const auto& inst : func_disasm.instructions) {
        if (boundaries.count(inst.address.virtual_address)) {
            current_block_addr = inst.address.virtual_address;
            if (!blocks.count(current_block_addr)) {
                auto block = std::make_shared<BasicBlock>();
                block->id = blocks.size();
                block->start_address = current_block_addr;
                block->label = "block_" + std::to_string(current_block_addr);
                blocks[current_block_addr] = block;
            }
            current_block = blocks[current_block_addr];
        }
        
        if (current_block) {
            current_block->addInstruction(translateInstruction(inst, inst_id++));
        }
    }
    
    return blocks;
}

void CFGBuilder::linkBlocks(
    FunctionRef function,
    const FunctionDisassembly& func_disasm,
    const std::map<uint64_t, BlockRef>& blocks) {
    
    for (const auto& [addr, block] : blocks) {
        function->blocks.push_back(block);
        function->block_by_address[addr] = block;
    }
    
    if (!function->blocks.empty()) {
        function->entry_block = function->blocks[0];
    }
    
    for (size_t i = 0; i < func_disasm.instructions.size(); ++i) {
        const auto& inst = func_disasm.instructions[i];
        
        auto it = blocks.find(inst.address.virtual_address);
        if (it == blocks.end()) continue;
        
        BlockRef current_block = it->second;
        
        if (inst.is_branch && inst.branch_target.has_value()) {
            auto target_it = blocks.find(inst.branch_target.value());
            if (target_it != blocks.end()) {
                current_block->successors.push_back(target_it->second);
                target_it->second->predecessors.push_back(current_block);
            }
        }
        
        if (!inst.is_return && !inst.is_branch) {
            uint64_t next_addr = inst.address.virtual_address + inst.length;
            auto next_it = blocks.find(next_addr);
            if (next_it != blocks.end() && next_it->second != current_block) {
                current_block->successors.push_back(next_it->second);
                next_it->second->predecessors.push_back(current_block);
            }
        }
    }
}

IROpcode CFGBuilder::translateOpcode(const DecodedInstruction& inst) {
    if (inst.mnemonic == "mov" || inst.mnemonic == "movq") return IROpcode::Move;
    if (inst.mnemonic == "add") return IROpcode::Add;
    if (inst.mnemonic == "sub") return IROpcode::Sub;
    if (inst.mnemonic == "mul" || inst.mnemonic == "imul") return IROpcode::Mul;
    if (inst.mnemonic == "div" || inst.mnemonic == "idiv") return IROpcode::Div;
    if (inst.mnemonic == "and") return IROpcode::And;
    if (inst.mnemonic == "or") return IROpcode::Or;
    if (inst.mnemonic == "xor") return IROpcode::Xor;
    if (inst.mnemonic == "not") return IROpcode::Not;
    if (inst.mnemonic == "shl" || inst.mnemonic == "sal") return IROpcode::Shl;
    if (inst.mnemonic == "shr" || inst.mnemonic == "sar") return IROpcode::Shr;
    if (inst.mnemonic == "cmp" || inst.mnemonic == "test") return IROpcode::Cmp;
    if (inst.mnemonic == "jmp") return IROpcode::Branch;
    if (inst.mnemonic == "je" || inst.mnemonic == "jz") return IROpcode::BranchCond;
    if (inst.mnemonic == "jne" || inst.mnemonic == "jnz") return IROpcode::BranchCond;
    if (inst.mnemonic == "call") return IROpcode::Call;
    if (inst.mnemonic == "ret" || inst.mnemonic == "retn") return IROpcode::Ret;
    if (inst.mnemonic == "push") return IROpcode::Push;
    if (inst.mnemonic == "pop") return IROpcode::Pop;
    if (inst.mnemonic == "nop") return IROpcode::Nop;
    if (inst.mnemonic == "lea") return IROpcode::GetAddress;
    return IROpcode::Nop;
}

IRInstruction CFGBuilder::translateInstruction(const DecodedInstruction& inst, uint64_t id) {
    IRInstruction ir_inst;
    ir_inst.id = id;
    ir_inst.opcode = translateOpcode(inst);
    ir_inst.address = inst.address;
    
    if (inst.is_branch && inst.branch_target.has_value()) {
        ir_inst.operands.push_back(Value::makeLabel(inst.branch_target.value(), ""));
    }
    
    return ir_inst;
}

}
