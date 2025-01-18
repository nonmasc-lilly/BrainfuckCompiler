#ifndef X__BF_COMPILE_H__X
#define X__BF_COMPILE_H__X
#include <stdint.h>
#include "parse.h"

typedef uint8_t BF_TARGET; enum {
        BF_TARGET_C             =       0x00,
        BF_TARGET_X64_LINUX_ASM =       0x01
};

typedef struct {
        uint8_t *bytes;
        uint32_t length;
} BF_BYTE_BUFFER;

typedef struct {
        BF_BYTE_BUFFER *ret;
        const BF_NODE  *current;
        BF_TARGET       target;
} BF_CSTATE;

void bf_byte_buffer_create(BF_BYTE_BUFFER *buffer);
void bf_byte_buffer_destroy(BF_BYTE_BUFFER *buffer);
void bf_byte_buffer_pushb(BF_BYTE_BUFFER *buffer, uint8_t  byte);
void bf_byte_buffer_pushw(BF_BYTE_BUFFER *buffer, uint16_t word);
void bf_byte_buffer_pushd(BF_BYTE_BUFFER *buffer, uint32_t doub);
void bf_byte_buffer_pushq(BF_BYTE_BUFFER *buffer, uint64_t quad);
void bf_byte_buffer_pushs(BF_BYTE_BUFFER *buffer, const uint8_t *str);

void bf_compile_add(BF_CSTATE *state);
void bf_compile_mov(BF_CSTATE *state);
void bf_compile_lop(BF_CSTATE *state);
void bf_compile_inp(BF_CSTATE *state);
void bf_compile_out(BF_CSTATE *state);
void bf_compile_statement(BF_CSTATE *state);
void bf_compile_program(BF_BYTE_BUFFER *ret, const BF_NODE *tree, BF_TARGET target);

#endif
