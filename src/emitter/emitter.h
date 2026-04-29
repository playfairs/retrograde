#pragma once

#include "../decompiler/decompiler.h"
#include <ostream>

namespace retrograde::emitter {

using namespace decompiler;

class Emitter {
public:
    void emit(const PseudoFunction& func, std::ostream& out);
    void emit(const std::vector<PseudoFunction>& funcs, std::ostream& out);

private:
    void emitFunction(const PseudoFunction& func, std::ostream& out);
    void emitBlock(const PseudoBlock& block, std::ostream& out, int indent);
    void emitInstruction(const PseudoInstruction& inst, std::ostream& out, int indent);
};

}
