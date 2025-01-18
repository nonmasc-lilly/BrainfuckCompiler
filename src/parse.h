#ifndef X__BRAINFUCK_PARSE_H__X
#define X__BRAINFUCK_PARSE_H__X
#include <stdint.h>
#include "lex.h"

typedef uint8_t BF_NODE_TYPE; enum {
        BF_NODE_TYPE_NUL        =       0x00,
        BF_NODE_TYPE_ROT        =       0x01,
        BF_NODE_TYPE_ADD        =       0x02,
        BF_NODE_TYPE_MOV        =       0x03,
        BF_NODE_TYPE_LOP        =       0x04,
        BF_NODE_TYPE_INP        =       0x05,
        BF_NODE_TYPE_OUT        =       0x06
};

typedef struct bf_node {
        struct bf_node        **children;
        uint32_t                children_length;
        int16_t                 value;
        BF_NODE_TYPE            type;
} BF_NODE;

typedef struct {
        BF_NODE               **ret;
        BF_NODE               **current;
        const BF_TOKENS        *tokens;
        uint32_t                index;
} BF_PSTATE;

const char *bf_node_type_represent(BF_NODE_TYPE type);

void bf_node_create(BF_NODE **ret, BF_NODE_TYPE type, int16_t value);
void bf_node_destroy(BF_NODE **ret);
void bf_node_append(BF_NODE **ret, BF_NODE_TYPE type, int16_t value);
void bf_node_print(FILE *fp, const BF_NODE **input, uint32_t index);

uint8_t bf_parse_out(BF_PSTATE *state);
uint8_t bf_parse_inp(BF_PSTATE *state);
uint8_t bf_parse_lop(BF_PSTATE *state);
uint8_t bf_parse_mov(BF_PSTATE *state);
uint8_t bf_parse_add(BF_PSTATE *state);
uint8_t bf_parse_statement(BF_PSTATE *state);
uint8_t bf_parse_tokens(BF_NODE **ret, const BF_TOKENS *tokens);

#endif
