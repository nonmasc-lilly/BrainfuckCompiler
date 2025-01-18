#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "parse.h"
#include "compile.h"

void bf_byte_buffer_create(BF_BYTE_BUFFER *buffer) {
        buffer->bytes = malloc(1);
        buffer->length = 0;
}
void bf_byte_buffer_destroy(BF_BYTE_BUFFER *buffer) {
        free(buffer->bytes);
        memset(buffer, 0, sizeof(*buffer));
}
void bf_byte_buffer_pushb(BF_BYTE_BUFFER *buffer, uint8_t  byte) {
        buffer->bytes = realloc(buffer->bytes, buffer->length += 1);
        buffer->bytes[buffer->length - 1] = byte;
}
void bf_byte_buffer_pushw(BF_BYTE_BUFFER *buffer, uint16_t word) {
        buffer->bytes = realloc(buffer->bytes, buffer->length += 2);
        *(uint16_t*)(buffer->bytes + buffer->length - 2) = word;
}
void bf_byte_buffer_pushd(BF_BYTE_BUFFER *buffer, uint32_t doub) {
        buffer->bytes = realloc(buffer->bytes, buffer->length += 4);
        *(uint32_t*)(buffer->bytes + buffer->length - 4) = doub;
}
void bf_byte_buffer_pushq(BF_BYTE_BUFFER *buffer, uint64_t quad) {
        buffer->bytes = realloc(buffer->bytes, buffer->length += 8);
        *(uint64_t*)(buffer->bytes + buffer->length - 8) = quad;
}
void bf_byte_buffer_pushs(BF_BYTE_BUFFER *buffer, const uint8_t *bytes) {
        uint32_t i;
        for(i = 0; bytes[i]; i++) {
                bf_byte_buffer_pushb(buffer, bytes[i]);
        }
}

void bf_compile_add(BF_CSTATE *state) {
        char *string;
        if(state->current->type != BF_NODE_TYPE_ADD) return;
        switch(state->target) {
        case BF_TARGET_C:
                #define CADD_STR "memory[mem_ptr] += %d;\n"
                string = malloc(strlen(CADD_STR)+20);
                sprintf(string, CADD_STR, state->current->value);
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)string);
                free(string);
                break;
        case BF_TARGET_X64_LINUX_ASM:
                #define X64LXASMADD_STR "add byte [rsp+rbx], %d\n"
                string = malloc(strlen(X64LXASMADD_STR)+20);
                sprintf(string, X64LXASMADD_STR, state->current->value);
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)string);
                free(string);
                break;
        }
}
void bf_compile_mov(BF_CSTATE *state) {
        char *string;
        if(state->current->type != BF_NODE_TYPE_MOV) return;
        switch(state->target) {
        case BF_TARGET_C:
                #define CMOV_STR "mem_ptr += %d;\n"
                string = malloc(strlen(CMOV_STR)+20);
                sprintf(string, CADD_STR, state->current->value);
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)string);
                free(string);
                break;
        case BF_TARGET_X64_LINUX_ASM:
                #define X64LXASMMOV_STR "add bx, %d\n"
                string = malloc(strlen(X64LXASMMOV_STR)+20);
                sprintf(string, X64LXASMMOV_STR, state->current->value);
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)string);
                free(string);
                break;
        }
}
void bf_compile_lop(BF_CSTATE *state) {
        uint32_t i, ploop_num;
        char *string;
        const BF_NODE *prev;
        static uint32_t loop_num = 0;
        if(state->current->type != BF_NODE_TYPE_LOP) return;
        switch(state->target) {
        case BF_TARGET_C:
                #define CWOP_STR "while(memory[mem_ptr]) {\n"
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)CWOP_STR);
                prev = state->current;
                for(i = 0; i < prev->children_length; i++) {
                        state->current = prev->children[i];
                        bf_compile_statement(state);
                }
                #define CWCL_STR "}\n"
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)CWCL_STR);
                break;
        case BF_TARGET_X64_LINUX_ASM:
                #define X64LXASMWOP_STR "loop%d:\n"\
                                        "cmp byte [rsp+rbx], 0x00\n"\
                                        "jz eloop%d\n"
                ploop_num = loop_num++;
                string = malloc(strlen(X64LXASMWOP_STR)+20);
                sprintf(string, X64LXASMWOP_STR, ploop_num, ploop_num);
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)string);
                free(string);
                prev = state->current;
                for(i = 0; i < prev->children_length; i++) {
                        state->current = prev->children[i];
                        bf_compile_statement(state);
                }
                #define X64LXASMWCL_STR "jmp loop%d\n"\
                                        "eloop%d:\n"
                string = malloc(strlen(X64LXASMWCL_STR)+20);
                sprintf(string, X64LXASMWCL_STR, ploop_num, ploop_num);
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)string);
                free(string);
                break;
        }
}
void bf_compile_inp(BF_CSTATE *state) {
        if(state->current->type != BF_NODE_TYPE_INP) return;
        switch(state->target) {
        case BF_TARGET_C:
                #define CINP_STR "memory[mem_ptr] = getchar();\n"
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)CINP_STR);
                break;
        case BF_TARGET_X64_LINUX_ASM:
                #define X64LXASMIN_STR  "xor eax, eax\n"\
                                        "xor edi, edi\n"\
                                        "lea rsi, [rsp+rbx]\n"\
                                        "mov edx, 0x01\n"\
                                        "syscall\n"
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)X64LXASMIN_STR);
                break;
        }
}
void bf_compile_out(BF_CSTATE *state) {
        if(state->current->type != BF_NODE_TYPE_OUT) return;
        switch(state->target) {
        case BF_TARGET_C:
                #define COUT_STR "putchar(memory[mem_ptr]);\n"
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)COUT_STR);
                break;
        case BF_TARGET_X64_LINUX_ASM:
                #define X64LXASMOUT_STR "mov eax, 0x01\n"\
                                        "mov edi, eax\n"\
                                        "lea rsi, [rsp+rbx]\n"\
                                        "mov edx, eax\n"\
                                        "syscall\n"
                bf_byte_buffer_pushs(state->ret, (const uint8_t*)X64LXASMOUT_STR);
                break;
        }
}
#define STATEMENT_NUMBER 5
void bf_compile_statement(BF_CSTATE *state) {
        uint32_t i;
        void (*functions[STATEMENT_NUMBER])(BF_CSTATE*) = {
                bf_compile_add, bf_compile_mov, bf_compile_lop,
                bf_compile_inp, bf_compile_out
        };
        for(i = 0; i < STATEMENT_NUMBER; i++)
                functions[i](state);
}
void bf_compile_program(BF_BYTE_BUFFER *ret, const BF_NODE *tree, BF_TARGET target) {
        uint32_t i;
        BF_CSTATE state;
        state.ret               = ret;
        state.current           = tree;
        state.target            = target;
        switch(target) {
        case BF_TARGET_C:
                #define CHEADER "#include <stdio.h>\n"\
                                "int main(int argc, char **argv) {\n"\
                                "unsigned char memory[0x10000] = {0};\n"\
                                "unsigned short mem_ptr = 0x00;\n"
                bf_byte_buffer_pushs(ret, (const uint8_t*)CHEADER);
                break;
        case BF_TARGET_X64_LINUX_ASM:
                #define X64LXASMHEADER  "format elf64 executable\n"\
                                        "entry _start\n"\
                                        "segment readable executable\n"\
                                        "_start:\n"\
                                        "sub rsp, 0x10000\n"\
                                        "xor ebx, ebx\n"
                bf_byte_buffer_pushs(ret, (const uint8_t*)X64LXASMHEADER);
                break;
        }
        for(i = 0; i < tree->children_length; i++) {
                state.current = tree->children[i];
                bf_compile_statement(&state);
        }
        switch(target) {
        case BF_TARGET_C:
                #define CFOOTER "return 0;\n"\
                                "}\n"
                bf_byte_buffer_pushs(ret, (const uint8_t*)CFOOTER);
                break;
        case BF_TARGET_X64_LINUX_ASM:
                #define X64LXASMFOOTER  "mov eax, 0x3C\n"\
                                        "xor edi, edi\n"\
                                        "syscall\n"
                bf_byte_buffer_pushs(ret, (const uint8_t*)X64LXASMFOOTER);
                break;
        }
}
