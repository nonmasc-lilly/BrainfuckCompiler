#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lex.h"

BF_TOKEN_TYPE bf_identify_char(char c) {
        switch(c) {
        case '+': return BF_TOKEN_TYPE_INC;
        case '-': return BF_TOKEN_TYPE_DEC;
        case '>': return BF_TOKEN_TYPE_NEX;
        case '<': return BF_TOKEN_TYPE_PRV;
        case '[': return BF_TOKEN_TYPE_OPN;
        case ']': return BF_TOKEN_TYPE_CLS;
        case ',': return BF_TOKEN_TYPE_INP;
        case '.': return BF_TOKEN_TYPE_OUT;
        default:  return BF_TOKEN_TYPE_NUL;
        }
}
const char *bf_token_type_represent(BF_TOKEN_TYPE type) {
        switch(type) {
        case BF_TOKEN_TYPE_NUL: return "NUL";
        case BF_TOKEN_TYPE_INC: return "INC +";
        case BF_TOKEN_TYPE_DEC: return "DEC -";
        case BF_TOKEN_TYPE_NEX: return "NEX >";
        case BF_TOKEN_TYPE_PRV: return "PRV <";
        case BF_TOKEN_TYPE_OPN: return "OPN [";
        case BF_TOKEN_TYPE_CLS: return "CLS ]";
        case BF_TOKEN_TYPE_INP: return "INP ,";
        case BF_TOKEN_TYPE_OUT: return "OUT .";
        case BF_TOKEN_TYPE_OAD: return "OAD +/-";
        case BF_TOKEN_TYPE_OMV: return "OMV >/<";
        default:                return "undefined";
        }
}

void bf_tokens_create(BF_TOKENS *ret) {
        ret->types  = malloc(1);
        ret->values = malloc(1);
        ret->length = 0;
}
void bf_tokens_destroy(BF_TOKENS *ret) {
        free(ret->types);
        free(ret->values);
        memset(ret, 0, sizeof(*ret));
}
void bf_tokens_append(BF_TOKENS *tokens, BF_TOKEN_TYPE type, int16_t value) {
        ++tokens->length;
        tokens->types  = realloc(tokens->types,  tokens->length *  sizeof(*tokens->types));
        tokens->values = realloc(tokens->values, tokens->length * sizeof(*tokens->values));
        tokens->types[tokens->length - 1]  =  type;
        tokens->values[tokens->length - 1] = value;
}
void bf_tokens_print(const BF_TOKENS *tokens) {
        uint32_t i;
        for(i = 0; i < tokens->length; i++) {
                fprintf(stderr, "%4X: %-15s :: %10d\n", i, bf_token_type_represent(tokens->types[i]), tokens->values[i]);
        }
}

uint32_t bf_lex_string(BF_TOKENS *tokens, const char *source) {
        uint32_t i, line, lines_length;
        uint32_t *lines;
        int16_t value;
        BF_TOKEN_TYPE type, ptype;

        line         = 1;
        value        = 0;
        lines_length = 0;
        ptype        = BF_TOKEN_TYPE_NUL;
        lines        = malloc(1);
        for(i = 0; source[i]; i++) {
                if(source[i] == '\n') line++;
                if(!(type = bf_identify_char(source[i]))) continue;
                if(type == BF_TOKEN_TYPE_INC || type ==  BF_TOKEN_TYPE_DEC) {
                        if(ptype != BF_TOKEN_TYPE_OAD) {
                                if(ptype) bf_tokens_append(tokens, ptype, value);
                                ptype = BF_TOKEN_TYPE_OAD;
                                value = 0;
                        }
                        value += type == BF_TOKEN_TYPE_INC ? 1 : -1;
                        continue;
                }
                if(type == BF_TOKEN_TYPE_NEX || type ==  BF_TOKEN_TYPE_PRV) {
                        if(ptype != BF_TOKEN_TYPE_OMV) {
                                if(ptype) bf_tokens_append(tokens, ptype, value);
                                ptype = BF_TOKEN_TYPE_OMV;
                                value = 0;
                        }
                        value += type == BF_TOKEN_TYPE_NEX ? 1 : -1;
                        continue;
                }
                if(type == BF_TOKEN_TYPE_OPN) {
                        lines = realloc(lines, ++lines_length * sizeof(*lines));
                        lines[lines_length - 1] = line;
                }
                if(type == BF_TOKEN_TYPE_CLS) --lines_length;
                if(ptype) {
                        bf_tokens_append(tokens, ptype, value);
                        ptype = BF_TOKEN_TYPE_NUL;
                }
                bf_tokens_append(tokens, type, 0);
        }
        for(i = 0; i < lines_length; i++)
                fprintf(stderr, "Unclosed `[` on line %d.\n", lines[i]);
        free(lines);
        return lines_length;
}
