#include "wrapper.h"

#include "../bin/loader/loader.h"
#include "../bin/loader/loader.cpp"
#include "../bin/sections/section.h"
#include "../bin/symbols/symbol.h"
#include "../disassemly/architecture/arch.h"
#include "../disassemly/architecture/arch.cpp"
#include "../disassemly/file/disassembler.h"
#include "../disassemly/file/disassembler.cpp"
#include "../analysis/cfg/cfg_builder.h"
#include "../analysis/cfg/cfg_builder.cpp"
#include "../decompiler/decompiler.h"
#include "../decompiler/decompiler.cpp"
#include "../emitter/emitter.h"
#include "../emitter/emitter.cpp"

#include <sstream>
#include <cstring>
#include <vector>
#include <string>

using namespace retrograde;

struct rg_function {
    std::string name;
    uint64_t address;
    size_t block_count;
    std::string pseudocode;
};

struct rg_result {
    rg_error_code error;
    std::string error_string;
    std::vector<rg_function> functions;
};

struct rg_context {
    bool verbose = false;
};

rg_context* rg_create(void) {
    return new rg_context;
}

void rg_destroy(rg_context* ctx) {
    delete ctx;
}

const char* rg_version(void) {
    return "0.1.0";
}

void rg_set_verbose(rg_context* ctx, int verbose) {
    if (ctx) {
        ctx->verbose = verbose != 0;
    }
}

static rg_result* decompile_internal(rg_context* ctx, bin::BinaryRef binary) {
    auto result = new rg_result;
    result->error = RG_OK;

    try {
        if (!binary) {
            result->error = RG_ERROR_INTERNAL;
            result->error_string = "Failed to load binary";
            return result;
        }

        auto code_section = binary->getCodeSection();
        if (!code_section) {
            result->error = RG_ERROR_PARSE_FAILED;
            result->error_string = "No code section found";
            return result;
        }

        const size_t MAX_SECTION_SIZE = 500 * 1024 * 1024;
        if (code_section->size > MAX_SECTION_SIZE) {
            result->error = RG_ERROR_PARSE_FAILED;
            result->error_string = "Code section too large (>500MB)";
            return result;
        }

        auto decoder = disasm::createDecoder(binary->config.arch);
        disasm::Disassembler disassembler(std::move(decoder));
        analysis::CFGBuilder cfg_builder;
        decompiler::Decompiler decompiler;

        bin::Symbol func_sym;
        func_sym.name = "main";
        func_sym.address = code_section->virtual_address;
        func_sym.size = code_section->size > 4096 ? 4096 : code_section->size;
        func_sym.type = bin::SymbolType::Function;

        std::vector<bin::Symbol> funcs = {func_sym};
        auto func_disasms = disassembler.disassembleFunctions(*code_section, funcs);

        const size_t MAX_FUNCTIONS = 100;
        size_t func_count = 0;
        for (const auto& fd : func_disasms) {
            if (func_count >= MAX_FUNCTIONS) break;
            
            auto ir_func = cfg_builder.buildCFG(fd);
            auto pseudo = decompiler.decompile(ir_func);

            rg_function func;
            func.name = pseudo.name;
            func.address = ir_func->entry_address;
            func.block_count = ir_func->blocks.size();
            func.pseudocode = decompiler.emitPseudoC(pseudo);

            result->functions.push_back(std::move(func));
            func_count++;
        }
    } catch (const std::exception& e) {
        result->error = RG_ERROR_INTERNAL;
        result->error_string = std::string("Decompilation error: ") + e.what();
    }

    return result;
}

rg_result* rg_decompile_file(rg_context* ctx, const char* filepath) {
    if (!ctx || !filepath) {
        auto result = new rg_result;
        result->error = RG_ERROR_INVALID_INPUT;
        result->error_string = "Invalid input";
        return result;
    }

    bin::Loader loader;
    auto load_result = loader.load(filepath);

    if (core::isError(load_result)) {
        auto result = new rg_result;
        result->error = RG_ERROR_FILE_NOT_FOUND;
        result->error_string = core::getError(load_result).message;
        return result;
    }

    auto binary = core::getValue(load_result);
    return decompile_internal(ctx, binary);
}

rg_result* rg_decompile_buffer(rg_context* ctx, const uint8_t* data, size_t len, uint64_t base_addr) {
    if (!ctx || !data || len == 0) {
        auto result = new rg_result;
        result->error = RG_ERROR_INVALID_INPUT;
        result->error_string = "Invalid input";
        return result;
    }

    auto binary = std::make_shared<bin::Binary>();
    binary->arch = core::Architecture::X64;
    binary->config.arch = core::Architecture::X64;
    binary->config.pointer_size = 8;
    binary->entry_point = base_addr;

    auto section = std::make_shared<bin::Section>();
    section->name = ".text";
    section->type = bin::SectionType::Code;
    section->virtual_address = base_addr;
    section->size = len;
    section->data.assign(data, data + len);
    binary->sections.push_back(section);

    return decompile_internal(ctx, binary);
}

void rg_result_free(rg_result* result) {
    delete result;
}

rg_error_code rg_get_error(const rg_result* result) {
    return result ? result->error : RG_ERROR_INTERNAL;
}

const char* rg_get_error_string(const rg_result* result) {
    return result ? result->error_string.c_str() : "";
}

size_t rg_get_function_count(const rg_result* result) {
    return result ? result->functions.size() : 0;
}

rg_function* rg_get_function(rg_result* result, size_t index) {
    if (!result || index >= result->functions.size()) {
        return nullptr;
    }
    return &result->functions[index];
}

const char* rg_function_get_name(const rg_function* func) {
    return func ? func->name.c_str() : "";
}

uint64_t rg_function_get_address(const rg_function* func) {
    return func ? func->address : 0;
}

size_t rg_function_get_block_count(const rg_function* func) {
    return func ? func->block_count : 0;
}

const char* rg_function_get_pseudocode(const rg_function* func) {
    return func ? func->pseudocode.c_str() : "";
}
