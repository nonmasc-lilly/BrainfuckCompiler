#ifndef X__BRAINFUCK_LEX_H__X
#define X__BRAINFUCK_LEX_H__X
#include <stdint.h>

typedef uint8_t BF_TOKEN_TYPE; enum {
        BF_TOKEN_TYPE_NUL       =       0x00,
        BF_TOKEN_TYPE_INC       =       0x01,
        BF_TOKEN_TYPE_DEC       =       0x02,
        BF_TOKEN_TYPE_NEX       =       0x03,
        BF_TOKEN_TYPE_PRV       =       0x04,
        BF_TOKEN_TYPE_OPN       =       0x05,
        BF_TOKEN_TYPE_CLS       =       0x06,
        BF_TOKEN_TYPE_INP       =       0x07,
        BF_TOKEN_TYPE_OUT       =       0x08,
        
        BF_TOKEN_TYPE_OAD       =       0x09, /* Optimized INC/DEC */
        BF_TOKEN_TYPE_OMV       =       0x0A  /* Optimized NEX/PRV */
};

typedef struct {
        BF_TOKEN_TYPE  *types;
        int16_t        *values;
        uint32_t        length;
} BF_TOKENS;

BF_TOKEN_TYPE bf_identify_char(char c);
const char *bf_token_type_represent(BF_TOKEN_TYPE type);

void bf_tokens_create(BF_TOKENS *ret);
void bf_tokens_destroy(BF_TOKENS *ret);
void bf_tokens_append(BF_TOKENS *tokens, BF_TOKEN_TYPE type, int16_t value);
void bf_tokens_print(const BF_TOKENS *tokens);

uint32_t bf_lex_string(BF_TOKENS *tokens, const char *source);

#endif
