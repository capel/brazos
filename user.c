#ifndef USER_H
#define USER_H

#define USER

#include "user_syscalls.h"
#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"
#include "vector.h"

void exit(void) {
    _exit();
}

typedef bool (*readline_func)(char*);


void erase_chars(size_t num) {
    for(; num > 0; num--) {
        printf("\b \b");
    }
}

bool parse_line(char* line) {
    vector * v = split_to_vector(line, " ");
    switch (((char*)v->data[0])[0]) {
        case '\0':
            break;
        case 'b':
            if (strncmp(v->data[0], "bc", strlen(v->data[0])) == 0) {
                println("Starting bc");
                exec("bc");
                goto cleanup;
            }
        case 'q':
            if (strncmp(v->data[0], "quit", strlen(v->data[0])) == 0) {
                println("Goodbye.");
                exit();
            }
            goto unknown;
        case 'e':
            if (strncmp(v->data[0], "echo", strlen(v->data[0])) == 0) {
                print_vector(v, "%s ", 1);
                println("");
                goto cleanup;
            } else if (strncmp(v->data[0], "exit", strlen(v->data[0])) == 0) {
                println("Goodbye.");
                exit();
            }
            goto unknown;
            
        case 'y':
            if (strncmp(v->data[0], "yield", strlen(v->data[0])) == 0) {
                yield();
                goto cleanup;
            }
            goto unknown;
        default:
        unknown:
            println("Unknown commmand '%s'", v->data[0]);
            goto cleanup;
    }
    cleanup:
    cleanup_vector(v);
    return true;
}
void sh_main()
{
    mem_init(100);
    printf("Hello\n");
    readline_lib("brazos> ", parse_line);
}
    

void readline_lib(const char * prompt, readline_func func) {
    vector * history = make_vector(sizeof(char*), MANAGED_POINTERS);
    size_t history_pos = 0;
    size_t len;
    char * line;

    bool fresh_line = true;
    int line_offset = 0;

    for(;;) {
        if (fresh_line) {
            line = calloc(100, 1);
            printf("%s", prompt);
            line_offset = 0;
        }

        int reason = getline(line + line_offset, 100);
       

        switch (reason) {
            case ARROW_LEFT:
                putc(ARROW_LEFT);
                fresh_line = false;
                line_offset = strlen(line)-1;
                break;
            case ARROW_RIGHT:
                putc(ARROW_RIGHT);
                fresh_line = false;
                line_offset = strlen(line)+1;
                break;
            case ARROW_UP:
                
                if (history_pos == 0) {
                    fresh_line = false;
                    line_offset = strlen(line);
                } else {
                    len = strlen(line);
                    erase_chars(len);
                    history_pos--;
                    line = history->data[history_pos];
                    printf("%s", line);
                    fresh_line = false;
                    line_offset = strlen(line);
                }
                break;
            case ARROW_DOWN:
                if (history_pos == history->size -1) {
                    fresh_line = false;
                    line_offset = strlen(line);
                } else {
                    len = strlen(line);
                    erase_chars(len);
                    history_pos++;
                    line = history->data[history_pos];
                    printf("%s", line);
                    fresh_line = false;
                    line_offset = strlen(line);
                }
                break;
            case NEWLINE:
                len = strlen(line);
                vector_push(history, line);
                history_pos++;
                func(line);
                line[len-1] = '\0';
                fresh_line = true;
            default:
                break;
        }
    }
}

bool bc_parse(char* line) {
    //vector * v = split_to_vector(line, " ");
    //if (v->size == 1) {
    printf("=%s", line);           

    return true;
}


void bc_main()
{
    mem_init(10);

    readline_lib("BC> ", bc_parse);
}
#endif
