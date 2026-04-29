#pragma once

#include "../../ir/function.h"
#include <set>
#include <map>

namespace retrograde::analysis {

using namespace retrograde::ir;

struct DataFlowState {
    std::map<uint64_t, std::set<uint64_t>> reaching_definitions;
    std::map<uint64_t, std::set<uint64_t>> live_variables;
};

class DataFlowAnalysis {
public:
    DataFlowState analyze(FunctionRef function);
};

}
