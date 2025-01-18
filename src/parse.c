#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lex.h"
#include "parse.h"

const char *bf_node_type_represent(BF_NODE_TYPE type) {
        switch(type) {
        case BF_NODE_TYPE_NUL:  return "NUL";
        case BF_NODE_TYPE_ROT:  return "ROOT";
        case BF_NODE_TYPE_ADD:  return "ADD";
        case BF_NODE_TYPE_MOV:  return "MOVE";
        case BF_NODE_TYPE_LOP:  return "LOOP";
        case BF_NODE_TYPE_INP:  return "INPUT";
        case BF_NODE_TYPE_OUT:  return "OUTPUT";
        default:                return "Unknown.";
        }
}

void bf_node_create(BF_NODE **ret, BF_NODE_TYPE type, int16_t value) {
        *ret = malloc(sizeof(**ret));
        (*ret)->type = type;
        (*ret)->value = value;
        (*ret)->children = malloc(1);
        (*ret)->children_length = 0;
}
void bf_node_destroy(BF_NODE **ret) {
        uint32_t i;
        for(i = 0; i < (*ret)->children_length; i++)
                bf_node_destroy((*ret)->children+i);
        free((*ret)->children);
        memset(*ret, 0, sizeof(**ret));
}
void bf_node_append(BF_NODE **ret, BF_NODE_TYPE type, int16_t value) {
        (*ret)->children = realloc((*ret)->children, ++(*ret)->children_length * sizeof(*(*ret)->children));
        bf_node_create((*ret)->children + (*ret)->children_length - 1, type, value);
}
void bf_node_print(FILE *out, const BF_NODE **input, uint32_t index) {
        uint32_t i;
        for(i = 0; i < index; i++) {
                fprintf(out, "|");
        }
        fprintf(out, "> %s :: %d\n", bf_node_type_represent((*input)->type), (*input)->value);
        for(i = 0; i < (*input)->children_length; i++)
                bf_node_print(out, (const BF_NODE**)(*input)->children+i, index+1);
}

uint8_t bf_parse_add(BF_PSTATE *state) {
        if(state->tokens->types[state->index] != BF_TOKEN_TYPE_OAD) return 0x00;
        bf_node_append(state->current, BF_NODE_TYPE_ADD, state->tokens->values[state->index]);
        ++state->index;
        return 0x01;
}
uint8_t bf_parse_mov(BF_PSTATE *state) {
        if(state->tokens->types[state->index] != BF_TOKEN_TYPE_OMV) return 0x00;
        bf_node_append(state->current, BF_NODE_TYPE_MOV, state->tokens->values[state->index]);
        ++state->index;
        return 0x01;
}
uint8_t bf_parse_lop(BF_PSTATE *state) {
        BF_NODE **prev;
        if(state->tokens->types[state->index] != BF_TOKEN_TYPE_OPN) return 0x00;
        ++state->index;
        bf_node_append(state->current, BF_NODE_TYPE_LOP, 0x00);
        prev = state->current;
        state->current = &((*state->current)->children[(*state->current)->children_length - 1]);
        while(state->tokens->types[state->index] != BF_TOKEN_TYPE_CLS) {
                if(!bf_parse_statement(state)) {
                        fprintf(stderr, "\033[1;31mCompiler Error:\033[m Statement not found in loop.\n");
                        return 0x00;
                }
        }
        state->current = prev;
        ++state->index;
        return 0x01;
}
uint8_t bf_parse_inp(BF_PSTATE *state) {
        if(state->tokens->types[state->index] != BF_TOKEN_TYPE_INP) return 0x00;
        bf_node_append(state->current, BF_NODE_TYPE_INP, 0x00);
        ++state->index;
        return 0x01;
}
uint8_t bf_parse_out(BF_PSTATE *state) {
        if(state->tokens->types[state->index] != BF_TOKEN_TYPE_OUT) return 0x00;
        bf_node_append(state->current, BF_NODE_TYPE_OUT, 0x00);
        ++state->index;
        return 0x01;
}

#define STATEMENT_NUMBER 5
uint8_t bf_parse_statement(BF_PSTATE *state) {
        uint8_t i;
        uint8_t (*functions[STATEMENT_NUMBER])(BF_PSTATE *) = {
                bf_parse_add, bf_parse_mov, bf_parse_lop,
                bf_parse_inp, bf_parse_out
        };
        for(i = 0; i < STATEMENT_NUMBER; i++)
                if(functions[i](state)) return 0x01;
        return 0x00;
}

uint8_t bf_parse_tokens(BF_NODE **ret, const BF_TOKENS *tokens) {
        BF_PSTATE state;
        state.ret     = ret;
        state.tokens  = tokens;
        state.current = state.ret;
        state.index   = 0;
        bf_node_create(ret, BF_NODE_TYPE_ROT, 0);
        while(state.index < tokens->length) if(!bf_parse_statement(&state)) {
                fprintf(stderr, "\033[1;31mCompiler Error:\033[m Statement not found.\n");
                bf_node_destroy(ret);
                return 0x00;
        }
        return 0x01;
}
