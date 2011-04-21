#ifndef USER_H
#define USER_H

#define USER

#include "user.h"
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
    if (strlen(line) == 0)
        return false;

    vector * v = split_to_vector(line, " ");
    switch (((char*)v->data[0])[0]) {
        case '\0':
            break;
        case 'q':
            if (strcmp(v->data[0], "quit") == 0) {
                println("Goodbye.");
                exit();
            }
            goto unknown;
        case 'e':
            if (strcmp(v->data[0], "echo") == 0) {
                print_vector(v, "%s ", 1);
                println("");
                goto cleanup;
            } else if (strcmp(v->data[0], "exit") == 0) {
                println("Goodbye.");
                exit();
            }
            goto unknown;
        case 'b':
            if (strcmp(v->data[0], "bc") == 0) {
                int pid = forkexec("bc");
                wait(pid);
                goto cleanup;
            }
            goto unknown;
        case 'y':
            if (strcmp(v->data[0], "yield") == 0) {
                yield();
                goto cleanup;
            }
            goto unknown;
        case 'w':
            if (strcmp(v->data[0], "write") == 0) {
                if (v->size < 4) {
                    println("write: write <file> <pos> <data>");
                    goto cleanup;
                }
                int fd = open(v->data[1]);
                if (fd < 0) {
                    println("bad filename");
                    goto cleanup;
                }
                seek(fd, atoi(v->data[2]), SEEK_ABS);
                write(fd, v->data[3], strlen(v->data[3]));
                close(fd);
                goto cleanup;
            }
            goto unknown;
        case 'p':
            if (strcmp(v->data[0], "pwd") == 0) {
                char * name = malloc(FILENAME_LEN);
                get_cwd(name, FILENAME_LEN);
                println("%s", name);
                
                free(name);
                goto cleanup;
            }
            goto unknown;
        case 't':
            if (strcmp(v->data[0], "touch") == 0) {
                if (v->size < 2) {
                    println("touch: touch <file>");
                    goto cleanup;
                }

                int fd = create(v->data[1], CREATE_FILE);
                goto cleanup;
            }
            goto unknown;
        case 'c':
            if (strcmp(v->data[0], "cd") == 0) {
                if (v->size < 2) {
                    println("cd: cd <dir>");
                    goto cleanup;
                }

                if (set_cwd(v->data[1]) < 0) {
                    println("bad directory");
                }
                goto cleanup;
            } else if (strcmp(v->data[0], "cat") == 0) {
                if (v->size < 2) {
                    println("cat: cat <file>");
                    goto cleanup;
                }
                int fd = open(v->data[1]);
                if (fd < 0) {
                    println("bad filename");
                    goto cleanup;
                }
                char* buf = malloc(1024);
                read(fd, buf, 1024);
                close(fd);
                println("%s", buf);
                free(buf);
                goto cleanup;
            }
            goto unknown;
            
        case 'm':
            if (strcmp(v->data[0], "mkdir") == 0) {
                if (v->size < 2) {
                    println("mkdir: mkdir <file>");
                    goto cleanup;
                }

                int fd = create(v->data[1], CREATE_DIR);
                goto cleanup;
            }
            goto unknown;
        case 'l':
            if (strcmp(v->data[0], "ls") == 0) {
                user_dir_entry* space = malloc(GET_DIR_ENTRIES_SPACE);
                get_dir_entries(space, GET_DIR_ENTRIES_SPACE);
                for(int i = 0; i < FILES_PER_DIR; i++) {
                    if (strlen(space[i].name) > 0)
                        println(space[i].name);
                }
                free(space);
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

int sh_main()
{
    mem_init(100);
    printf("Hello\n");
    create("ll", CREATE_FILE);
    create("woot", CREATE_FILE);
    readline_lib("brazos> ", parse_line);
    return 0;
}
    


bool bc_parse(char* line) {
    vector * v = split_to_vector(line, " ");
    if (v->size == 1) {
        if (strcmp(v->data[0], "exit") == 0) {
            println("Goodbye.");
            exit();
        }
        println("%d", atoi(v->data[0]));
    } else if (v->size == 2) {
        println("Parse error: 1 or 3 arguments required.");
        goto cleanup;
    } else if (v->size == 3) {
        if (strlen(v->data[1]) != 1 || !isdigits(v->data[0]) || !isdigits(v->data[2])) {
            println("Parse error: Badly formatted numbers or bad operand.");
            goto cleanup;
        }

        switch (((char*)v->data[1])[0]) {
            case '+':
                println("= %d", atoi(v->data[0]) + atoi(v->data[2]));
                goto cleanup;
            case '*':
                println("= %d", atoi(v->data[0]) * atoi(v->data[2]));
                goto cleanup;
            case '-':
                println("= %d", atoi(v->data[0]) - atoi(v->data[2]));
                goto cleanup;
            case '/':
                println("Division not supported");
                //println("= %d", atoi(v->data[0]) / atoi(v->data[2]));
                goto cleanup;
            case '^':
                println("= %d", power(atoi(v->data[0]), atoi(v->data[2])));
                goto cleanup;
            default:
                println("Parse error: Unknown operand %s", v->data[1]);
                goto cleanup;
        }   
    }
    else {
        println("Parse error: Too many arguments");
        goto cleanup;
    }

    cleanup:
        cleanup_vector(v);
        return true;
}


int bc_main()
{
    mem_init(10);

    readline_lib("BC> ", bc_parse);
    return 0;
}
#endif
