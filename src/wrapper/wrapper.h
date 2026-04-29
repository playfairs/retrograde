#ifndef RETROGRADE_H
#define RETROGRADE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef struct rg_context rg_context;
typedef struct rg_result rg_result;
typedef struct rg_function rg_function;

typedef enum {
    RG_OK = 0,
    RG_ERROR_INVALID_INPUT = 1,
    RG_ERROR_FILE_NOT_FOUND = 2,
    RG_ERROR_PARSE_FAILED = 3,
    RG_ERROR_UNSUPPORTED_ARCH = 4,
    RG_ERROR_INTERNAL = 5
} rg_error_code;

rg_context* rg_create(void);
void rg_destroy(rg_context* ctx);
const char* rg_version(void);

rg_result* rg_decompile_file(rg_context* ctx, const char* filepath);
rg_result* rg_decompile_buffer(rg_context* ctx, const uint8_t* data, size_t len, uint64_t base_addr);
void rg_result_free(rg_result* result);

rg_error_code rg_get_error(const rg_result* result);
const char* rg_get_error_string(const rg_result* result);

size_t rg_get_function_count(const rg_result* result);
rg_function* rg_get_function(rg_result* result, size_t index);

const char* rg_function_get_name(const rg_function* func);
uint64_t rg_function_get_address(const rg_function* func);
size_t rg_function_get_block_count(const rg_function* func);
const char* rg_function_get_pseudocode(const rg_function* func);

void rg_set_verbose(rg_context* ctx, int verbose);

#ifdef __cplusplus
}
#endif

#endif
