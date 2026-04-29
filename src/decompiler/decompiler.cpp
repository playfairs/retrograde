#include "decompiler.h"

namespace retrograde::decompiler {

PseudoFunction Decompiler::decompile(FunctionRef ir_function) {
    PseudoFunction result;
    result.name = ir_function->name;
    result.return_type = "void";
    
    for (const auto& block_ref : ir_function->blocks) {
        PseudoBlock pb;
        pb.label = block_ref->label;
        
        for (const auto& inst : block_ref->instructions) {
            pb.instructions.push_back(translateToPseudo(inst));
        }
        
        result.blocks.push_back(std::move(pb));
    }
    
    return result;
}

std::string Decompiler::emitPseudoC(const PseudoFunction& func) {
    std::ostringstream oss;
    
    oss << func.return_type << " " << func.name << "(";
    for (size_t i = 0; i < func.parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "int param" << i;
    }
    oss << ") {\n";
    
    for (const auto& block : func.blocks) {
        oss << "  // " << block.label << "\n";
        for (const auto& inst : block.instructions) {
            oss << "  " << inst.operation;
            if (!inst.operands.empty()) {
                oss << " ";
                for (size_t i = 0; i < inst.operands.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << inst.operands[i];
                }
            }
            oss << ";\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

PseudoInstruction Decompiler::translateToPseudo(const IRInstruction& inst) {
    PseudoInstruction pi;
    pi.operation = opcodeToString(inst.opcode);
    
    for (const auto& op : inst.operands) {
        pi.operands.push_back(valueToString(op));
    }
    
    return pi;
}

std::string Decompiler::valueToString(const core::Value& val) {
    switch (val.type) {
        case core::ValueType::Register: {
            auto& reg = std::get<core::RegisterId>(val.data);
            return reg.name;
        }
        case core::ValueType::Constant: {
            auto& cv = std::get<core::ConstantValue>(val.data);
            if (std::holds_alternative<int64_t>(cv.value)) {
                return std::to_string(std::get<int64_t>(cv.value));
            }
            if (std::holds_alternative<uint64_t>(cv.value)) {
                return std::to_string(std::get<uint64_t>(cv.value));
            }
            return "0";
        }
        case core::ValueType::Memory:
            return "mem";
        case core::ValueType::Label: {
            auto& lbl = std::get<core::Label>(val.data);
            return "0x" + std::to_string(lbl.address);
        }
        default:
            return "?";
    }
}

std::string Decompiler::opcodeToString(core::IROpcode opcode) {
    switch (opcode) {
        case core::IROpcode::Nop: return "nop";
        case core::IROpcode::Load: return "load";
        case core::IROpcode::Store: return "store";
        case core::IROpcode::Move: return "assign";
        case core::IROpcode::Add: return "add";
        case core::IROpcode::Sub: return "sub";
        case core::IROpcode::Mul: return "mul";
        case core::IROpcode::Div: return "div";
        case core::IROpcode::Mod: return "mod";
        case core::IROpcode::And: return "and";
        case core::IROpcode::Or: return "or";
        case core::IROpcode::Xor: return "xor";
        case core::IROpcode::Not: return "not";
        case core::IROpcode::Shl: return "shl";
        case core::IROpcode::Shr: return "shr";
        case core::IROpcode::Cmp: return "cmp";
        case core::IROpcode::Branch: return "goto";
        case core::IROpcode::BranchCond: return "if_goto";
        case core::IROpcode::Call: return "call";
        case core::IROpcode::Ret: return "return";
        case core::IROpcode::Push: return "push";
        case core::IROpcode::Pop: return "pop";
        case core::IROpcode::Phi: return "phi";
        case core::IROpcode::Cast: return "cast";
        case core::IROpcode::GetAddress: return "addr";
        default: return "unknown";
    }
}

}
