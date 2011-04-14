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
        case 'q':
            if (strncmp(v->data[0], "quit", strlen(v->data[0])) == 0) {
                println("Goodbye.");
                exit();
                return true; // doesn't matter
            }
            goto unknown;
        case 'e':
            if (strncmp(v->data[0], "echo", strlen(v->data[0])) == 0) {
                print_vector(v, "%s ", 1);
                println("");
                return true;
            }
            goto unknown;
        case 'y':
            if (strncmp(v->data[0], "yield", strlen(v->data[0])) == 0) {
                yield();
                return true;
            }
            goto unknown;
        default:
        unknown:
            println("Unknown commmand %s", v->data[0]);
            return false;
    }
    cleanup_vector(v);
}
void main()
{
    mem_init(100);
    printf("Hello\n");
    vector * history = make_vector(sizeof(char*), MANAGED_POINTERS);
    size_t history_pos = 0;
    size_t len;
    char * line;

    bool fresh_line = true;
    int line_offset = 0;

    for(;;) {
        if (fresh_line) {
            line = calloc(100, 1);
            printf("brazos > ");
            line_offset = 0;
        }

        int reason = getline(line + line_offset, 100);
        
        switch (reason) {
            case ARROW_LEFT:
              //  debug("left");
                putc(ARROW_LEFT);
                fresh_line = false;
                line_offset = strlen(line)-1;
                break;
            case ARROW_RIGHT:
                //debug("right");
                putc(ARROW_RIGHT);
                fresh_line = false;
                line_offset = strlen(line)+1;
                break;
            case ARROW_UP:
                //debug("up");
                
                if (history_pos == 0) {
                    fresh_line = false;
                    line_offset = strlen(line);
                } else {
                    len = strlen(line);
                    erase_chars(len);
                    history_pos--;
                    free(line);
                    line = history->data[history_pos];
                    printf("%s", line);
                    fresh_line = false;
                    line_offset = strlen(line);
                }
                break;
            case ARROW_DOWN:
                //debug("down");
                len = strlen(line);
                break;
            case NEWLINE:
                vector_push(history, line);
                history_pos++;
                parse_line(line);
                fresh_line = true;
            default:
                break;
        }
    }
}


void main2()
{
    printf("!!!HelloB\n");
    char * page = get_pages(1); 
    printf("Page %p\n", page);
    for (;;) { printf("!"); }
}


#endif
