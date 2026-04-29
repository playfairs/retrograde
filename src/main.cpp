#include "bin/loader/loader.h"
#include "bin/loader/loader.cpp"
#include "bin/sections/section.h"
#include "bin/symbols/symbol.h"
#include "disassemly/architecture/arch.h"
#include "disassemly/architecture/arch.cpp"
#include "disassemly/file/disassembler.h"
#include "disassemly/file/disassembler.cpp"
#include "analysis/cfg/cfg_builder.h"
#include "analysis/cfg/cfg_builder.cpp"
#include "decompiler/decompiler.h"
#include "decompiler/decompiler.cpp"
#include "emitter/emitter.h"
#include "emitter/emitter.cpp"
#include "utils/utils.h"

#include <iostream>
#include <fstream>
#include <cstdio>

using namespace retrograde;

std::vector<uint8_t> createTestBinary() {
    std::vector<uint8_t> code;
    
    code.push_back(0x55);
    
    code.push_back(0x48);
    code.push_back(0x89);
    code.push_back(0xE5);
    
    code.push_back(0x90);
    code.push_back(0x90);
    
    code.push_back(0x5D);
    code.push_back(0xC3);
    
    return code;
}

int main(int argc, char* argv[]) {
    std::string input_file;
    
    if (argc > 1) {
        input_file = argv[1];
    } else {
        const char* test_path = "/tmp/test_binary.bin";
        auto code = createTestBinary();
        std::ofstream f(test_path, std::ios::binary);
        f.write(reinterpret_cast<const char*>(code.data()), code.size());
        f.close();
        input_file = test_path;
        std::cout << "No input provided. Using test binary: " << input_file << "\n\n";
    }
    
    bin::Loader loader;
    auto result = loader.load(input_file);
    
    if (core::isError(result)) {
        std::cerr << "Error loading binary: " << core::getError(result).message << "\n";
        return 1;
    }
    
    auto binary = core::getValue(result);
    
    std::cout << "=== Retrograde Decompiler Pipeline ===\n\n";
    std::cout << "Loaded: " << binary->filename << "\n";
    std::cout << "Architecture: " << static_cast<int>(binary->config.arch) << "\n";
    std::cout << "Entry point: 0x" << std::hex << binary->entry_point << std::dec << "\n";
    std::cout << "Sections: " << binary->sections.size() << "\n\n";
    
    auto code_section = binary->getCodeSection();
    if (!code_section) {
        std::cerr << "No code section found\n";
        return 1;
    }
    
    std::cout << "Code section: " << code_section->name << "\n";
    std::cout << "Size: " << code_section->size << " bytes\n\n";
    
    auto decoder = disasm::createDecoder(binary->config.arch);
    disasm::Disassembler disassembler(std::move(decoder));
    
    std::cout << "--- Disassembly ---\n";
    auto instructions = disassembler.disassembleSection(*code_section);
    for (const auto& inst : instructions) {
        std::cout << "0x" << std::hex << inst.address.virtual_address << std::dec << ":  ";
        std::cout << inst.mnemonic;
        if (!inst.operands.empty()) {
            std::cout << " " << inst.operands;
        }
        if (inst.branch_target) {
            std::cout << " (-> 0x" << std::hex << *inst.branch_target << std::dec << ")";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    
    bin::Symbol func_sym;
    func_sym.name = "main";
    func_sym.address = code_section->virtual_address;
    func_sym.size = code_section->size;
    func_sym.type = bin::SymbolType::Function;
    
    std::vector<bin::Symbol> funcs = {func_sym};
    auto func_disasms = disassembler.disassembleFunctions(*code_section, funcs);
    
    std::cout << "--- Building CFG ---\n";
    analysis::CFGBuilder cfg_builder;
    emitter::Emitter emitter;
    decompiler::Decompiler decompiler;
    
    std::vector<decompiler::PseudoFunction> pseudo_funcs;
    
    for (const auto& fd : func_disasms) {
        auto ir_func = cfg_builder.buildCFG(fd);
        
        std::cout << "Function: " << ir_func->name << "\n";
        std::cout << "  Blocks: " << ir_func->blocks.size() << "\n";
        for (const auto& block : ir_func->blocks) {
            std::cout << "    " << block->label << ": " << block->instructions.size() << " instructions\n";
        }
        std::cout << "\n";
        
        auto pseudo = decompiler.decompile(ir_func);
        pseudo_funcs.push_back(pseudo);
    }
    
    std::cout << "--- Decompiled Output ---\n";
    for (const auto& pf : pseudo_funcs) {
        std::cout << decompiler.emitPseudoC(pf) << "\n";
    }
    
    std::cout << "--- Emitted Output ---\n";
    emitter.emit(pseudo_funcs, std::cout);
    
    return 0;
}
