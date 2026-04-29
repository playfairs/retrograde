#pragma once

#include <string>
#include <variant>
#include <optional>

namespace retrograde::core {

struct Error {
    std::string message;
    std::string context;

    static Error make(const std::string& msg, const std::string& ctx = {}) {
        return Error{msg, ctx};
    }
};

template<typename T>
using Result = std::variant<T, Error>;

template<typename T>
bool isError(const Result<T>& result) {
    return std::holds_alternative<Error>(result);
}

template<typename T>
T getValue(Result<T>& result) {
    return std::get<T>(std::move(result));
}

template<typename T>
const T& getValue(const Result<T>& result) {
    return std::get<T>(result);
}

template<typename T>
const Error& getError(const Result<T>& result) {
    return std::get<Error>(result);
}

}
