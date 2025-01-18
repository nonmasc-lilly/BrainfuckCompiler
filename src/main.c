#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parse.h"
#include "compile.h"

struct cmd_opts {
        uint8_t bhelp, bdebug;
        uint32_t input, output, target;
};

void cmd_opts_get(struct cmd_opts *ret, uint32_t argc, const char **argv) {
        uint32_t i;
        ret->input  = 0x00;
        ret->output = 0x00;
        ret->target = 0x00;
        ret->bhelp  = 0x00;
        ret->bdebug = 0x00;
        for(i = 1; i < argc; i++) {
                if(argv[i][0] != '-') {
                        if(ret->input) {
                                fprintf(stderr, "\033[1;31mError:\033[m Multiple input files `%s` and `%s`.", argv[ret->input], argv[i]);
                                exit(1);
                        }
                        ret->input = i;
                        continue;
                }
                switch(argv[i][1]) {
                case 'h': ret->bhelp = 0x01; break;
                case 'o':
                        if(ret->output) {
                                fprintf(stderr, "\033[1;31mError:\033[m Multiple targets files `%s` and `%s`.", argv[ret->output], argv[i]);
                                exit(1);
                        }
                        ret->output = ++i;
                        break;
                case '-':
                             if(!strcmp(argv[i],   "--help")) ret->bhelp  = 0x01;
                        else if(!strcmp(argv[i],  "--debug")) ret->bdebug = 0x01;
                        else if(!strcmp(argv[i], "--target")) {
                                if(ret->target) {
                                        fprintf(stderr, "\033[1;31mError:\033[m Multiple targets files `%s` and `%s`.", argv[ret->target], argv[i]);
                                        exit(1);
                                }
                                ret->target = ++i;
                        } else goto unexpected;
                        break; 
                default:
                unexpected:
                        fprintf(stderr, "\033[1;31mError:\033[m Unexpected argument `%s`.", argv[i]);
                        exit(1);
                }
        }
}

void open_help(const char *argv0, uint32_t code) {
        fprintf(
                stdout,
                "Usage: %s [OPTIONS] <input file> [OPTIONS]\n"
                "OPTIONS:\n"
                "\t-h / --help          :: Open this menu and exit.\n"
                "\t-o <file>            :: Set the output file.\n"
                "\t--debug              :: Allow compiler debug text to be printed to stderr.\n"
                "\t--target <tgt>       :: Set the output target.\n"
                "\t tgt :=\n"
                "\t   C         :: The C programming language.\n"
                "\t   X64_LINUX :: X64 Linux FASM assembly.\n",
                argv0
        );
        exit(code);
}

int main(int argc, char **argv) {
        BF_TOKENS       lexed_tokens;
        BF_NODE        *parse_tree;
        BF_TARGET       target;
        BF_BYTE_BUFFER  buffer;
        struct cmd_opts options;
        struct {
                FILE *pointer;
                char *content;
                uint32_t length;
        } file;
        cmd_opts_get(&options, argc, (const char **)argv);
        if(options.bhelp) open_help(argv[0], 0);
        if(!options.input) {
                fprintf(stderr, "\033[1;31mError:\033[m Expected input.\n");
                open_help(argv[0], 1);
        }
        file.pointer = fopen(argv[options.input], "r");
        if(!file.pointer) {
                fprintf(stderr, "\033[1;31mError:\033[m Could not open `%s` for reading.\n", argv[options.input]);
                exit(1);
        }
        fseek(file.pointer, 0L, SEEK_END);
        file.length  = ftell(file.pointer);
        file.content = calloc(1, file.length + 1);
        fseek(file.pointer, 0L, SEEK_SET);
        fread(file.content, 1, file.length, file.pointer);
        fclose(file.pointer);

        bf_tokens_create(&lexed_tokens);
        if(bf_lex_string(&lexed_tokens, file.content)) {
                fprintf(stdout, "Exited with error.\n");
                bf_tokens_destroy(&lexed_tokens);
                exit(1);
        }
        if(options.bdebug) bf_tokens_print(&lexed_tokens);

        if(!bf_parse_tokens(&parse_tree, &lexed_tokens)) {
                fprintf(stdout, "Exited with error.\n");
                bf_tokens_destroy(&lexed_tokens);
                bf_node_destroy(&parse_tree);
                exit(1);
        }
        if(options.bdebug) bf_node_print(stderr, (const BF_NODE **)&parse_tree, 0x00);

             if(!options.target)                                target = BF_TARGET_C;
        else if(!strcmp(argv[options.target], "C"))             target = BF_TARGET_C;
        else if(!strcmp(argv[options.target], "X64_LINUX"))     target = BF_TARGET_X64_LINUX_ASM;

        bf_byte_buffer_create(&buffer);
        bf_compile_program(&buffer, parse_tree, target);
        if(options.bdebug) fprintf(stderr, "%s\n", buffer.bytes);

        file.pointer = fopen(options.output ? argv[options.output] : "./a.out", "w");
        if(!file.pointer) {
                fprintf(stderr, "\033[1;31mError:\033[m Could not open `%s` for writing.\n", options.output ? argv[options.output] : "./a.out");
                bf_tokens_destroy(&lexed_tokens);
                bf_node_destroy(&parse_tree);
                bf_byte_buffer_destroy(&buffer);
                exit(1);
        }
        fwrite(buffer.bytes, 1, buffer.length, file.pointer);
        fclose(file.pointer);

        bf_tokens_destroy(&lexed_tokens);
        bf_node_destroy(&parse_tree);
        bf_byte_buffer_destroy(&buffer);
        free(file.content);
        return 0;
}
