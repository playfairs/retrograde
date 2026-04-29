#pragma once

#include <cstdint>
#include <variant>
#include <string>
#include <vector>
#include <optional>

namespace retrograde::core {

enum class ValueType {
    Unknown,
    Register,
    Constant,
    Memory,
    Label
};

enum class PrimitiveType {
    Void,
    I8,
    I16,
    I32,
    I64,
    U8,
    U16,
    U32,
    U64,
    F32,
    F64,
    Pointer
};

struct RegisterId {
    uint32_t id;
    std::string name;

    bool operator==(const RegisterId& other) const {
        return id == other.id;
    }
};

struct MemoryAddress {
    std::optional<RegisterId> base;
    std::optional<RegisterId> index;
    int32_t scale = 1;
    int64_t displacement = 0;
};

struct Label {
    uint64_t address;
    std::string name;
};

struct ConstantValue {
    PrimitiveType type;
    std::variant<int64_t, uint64_t, double> value;
};

struct Value {
    ValueType type = ValueType::Unknown;
    PrimitiveType primitive = PrimitiveType::Void;
    std::variant<std::monostate, RegisterId, ConstantValue, MemoryAddress, Label> data;

    static Value makeRegister(uint32_t id, std::string name, PrimitiveType prim = PrimitiveType::I64) {
        Value v;
        v.type = ValueType::Register;
        v.primitive = prim;
        v.data = RegisterId{id, std::move(name)};
        return v;
    }

    static Value makeConstant(int64_t val, PrimitiveType prim = PrimitiveType::I64) {
        Value v;
        v.type = ValueType::Constant;
        v.primitive = prim;
        v.data = ConstantValue{prim, val};
        return v;
    }

    static Value makeConstant(uint64_t val, PrimitiveType prim = PrimitiveType::U64) {
        Value v;
        v.type = ValueType::Constant;
        v.primitive = prim;
        v.data = ConstantValue{prim, val};
        return v;
    }

    static Value makeMemory(const MemoryAddress& addr, PrimitiveType prim = PrimitiveType::I64) {
        Value v;
        v.type = ValueType::Memory;
        v.primitive = prim;
        v.data = addr;
        return v;
    }

    static Value makeLabel(uint64_t addr, std::string name) {
        Value v;
        v.type = ValueType::Label;
        v.primitive = PrimitiveType::Void;
        v.data = Label{addr, std::move(name)};
        return v;
    }
};

enum class IROpcode {
    Nop,
    Load,
    Store,
    Move,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    And,
    Or,
    Xor,
    Not,
    Shl,
    Shr,
    Cmp,
    Branch,
    BranchCond,
    Call,
    Ret,
    Push,
    Pop,
    Phi,
    Cast,
    GetAddress
};

enum class Condition {
    Always,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Zero,
    NotZero,
    Carry,
    NotCarry,
    Overflow,
    NotOverflow,
    SignedLess,
    SignedLessEqual,
    SignedGreater,
    SignedGreaterEqual
};

struct InstructionAddress {
    uint64_t raw = 0;
    uint64_t virtual_address = 0;
};

using InstructionBytes = std::vector<uint8_t>;

}
