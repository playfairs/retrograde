#include "emitter.h"
#include <iomanip>

namespace retrograde::emitter {

void Emitter::emit(const PseudoFunction& func, std::ostream& out) {
    emitFunction(func, out);
}

void Emitter::emit(const std::vector<PseudoFunction>& funcs, std::ostream& out) {
    for (const auto& func : funcs) {
        emitFunction(func, out);
        out << "\n";
    }
}

void Emitter::emitFunction(const PseudoFunction& func, std::ostream& out) {
    out << func.return_type << " " << func.name << "(";
    for (size_t i = 0; i < func.parameters.size(); ++i) {
        if (i > 0) out << ", ";
        out << "int param" << i;
    }
    out << ") {\n";
    
    for (const auto& block : func.blocks) {
        emitBlock(block, out, 2);
    }
    
    out << "}\n";
}

void Emitter::emitBlock(const PseudoBlock& block, std::ostream& out, int indent) {
    out << std::string(indent - 2, ' ') << "// Block: " << block.label << "\n";
    
    for (const auto& inst : block.instructions) {
        emitInstruction(inst, out, indent);
    }
}

void Emitter::emitInstruction(const PseudoInstruction& inst, std::ostream& out, int indent) {
    out << std::string(indent, ' ') << inst.operation;
    
    if (!inst.operands.empty()) {
        out << " ";
        for (size_t i = 0; i < inst.operands.size(); ++i) {
            if (i > 0) out << ", ";
            out << inst.operands[i];
        }
    }
    
    out << ";";
    
    if (!inst.comment.empty()) {
        out << " // " << inst.comment;
    }
    
    out << "\n";
}

}
