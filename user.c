#ifndef USER_H
#define USER_H

#define USER

#include "user.h"
#include "user_syscalls.h"
#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"
#include "vector.h"
#include "chars.h"

void exit(void) {
    _exit();
}

#define ARG(x) v->data[(x)]
#define IARG(x) atoi(v->data[(x)])

void perror(int error) {
    switch (error) {
        case E_ERROR:
            println("Error: Generic error.");
            break;
        case E_NOT_SUPPORTED:
            println("Error: Operation not supported here.");
            break;
        case E_BAD_FILENAME:
            println("Error: Bad filename.");
            break;
        case E_BAD_PROGRAM:
            println("Error: Bad program name.");
            break;
        case E_IS_DIR:
            println("Error: Is directory.");
            break;
        case E_BAD_FD:
            println("Error: Bad FD.");
            break;
        case E_ISNT_DIR:
            println("Error: Isn't directory.");
            break;
        case E_BAD_ARG:
            println("Error: Bad syscall argument.");
            break;
        case E_BAD_SYSCALL:
            println("Error: Bad syscall number.");
            break;
        default:
            break;
    }
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
                if (pid < 0) {
                    perror(pid);
                    goto cleanup;
                }
                wait(pid);
                goto cleanup;
            }
            goto unknown;
        case 'd':
            if (strcmp(v->data[0], "dummy") == 0) {
                int pid = forkexec("dummy");
                if (pid < 0) {
                    perror(pid);
                    goto cleanup;
                }
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
                    perror(fd);
                    goto cleanup;
                }
                int ret = seek(fd, atoi(v->data[2]), SEEK_ABS);
                if (ret < 0) {
                    perror(ret);
                    goto cleanup;
                }
                ret = write(fd, v->data[3], strlen(v->data[3]));
                if (ret < 0) {
                    perror(ret);
                    goto cleanup;
                }
              close(fd);
                goto cleanup;
            }
            goto unknown;
        case 'p':
            if (strcmp(v->data[0], "pwd") == 0) {
                char * name = malloc(FILENAME_LEN);
                int ret = get_cwd(name, FILENAME_LEN);
                if (ret < 0) {
                    perror(ret);
                    goto cleanup;
                }
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
                if (fd < 0) {
                    perror(fd);
                    goto cleanup;
                }
                goto cleanup;
            }
            goto unknown;
        case 'r':
            if (strcmp(v->data[0], "rm") == 0) {
                if (v->size < 2) {
                    println("rm: rm <file>");
                    goto cleanup;
                }

                int ret = unlink(v->data[1]);
                if (ret < 0) {
                    perror(ret);
                    goto cleanup;
                }
                goto cleanup;
            }
            goto unknown;
       case 'c':
            if (strcmp(v->data[0], "cd") == 0) {
                if (v->size < 2) {
                    println("cd: cd <dir>");
                    goto cleanup;
                }

                int ret = set_cwd(v->data[1]);
                if (ret < 0) {
                    perror(ret);
                    goto cleanup;
                }
                goto cleanup;
            } else if (strcmp(v->data[0], "cat") == 0) {
                if (v->size < 2) {
                    println("cat: cat <file>");
                    goto cleanup;
                }
                int fd = open(v->data[1]);
                debug("fd %d", fd);
                if (fd < 0) {
                    perror(fd);
                    goto cleanup;
                }
                char* buf = malloc(1024);
                int ret = read(fd, buf, 1024);
                if (ret < 0) {
                    free(buf);
                    perror(ret);
                    goto cleanup;
                }
                ret = close(fd);
                if (ret < 0) {
                    perror(ret);
                    goto cleanup;
                }
                println("%s", buf);
                free(buf);
                goto cleanup;
            }
            goto unknown;
            
        case 'm':
            if (strcmp(v->data[0], "map") == 0) {
              if (v->size < 2) {
                println("map: map <rid>");
                goto cleanup;
              }

              size_t out_size;
              void* ptr;
              if (0 != map(atoi(v->data[1]), &out_size, &ptr)) {
                  goto cleanup;
              }
              println("%s", ptr);
              goto cleanup;
            }
            if (strcmp(v->data[0], "mkdir") == 0) {
                if (v->size < 2) {
                    println("mkdir: mkdir <file>");
                    goto cleanup;
                }

                int fd = create(v->data[1], CREATE_DIR);
                if (fd < 0) {
                    perror(fd);
                    goto cleanup;
                }
                goto cleanup;
            }
            goto unknown;
        case 'l':
            if (strcmp(v->data[0], "lookup") == 0 || strcmp(v->data[0], "l") == 0) {
              int rid = lookup(v->data[1]);
              println("Rid: %d", rid);
              goto cleanup;
            }
            if (strcmp(v->data[0], "link") == 0) {
              link(IARG(1), IARG(2), ARG(3));
              goto cleanup;
            }
            if (strcmp(v->data[0], "ls") == 0) {
                user_dir_entry* space = malloc(GET_DIR_ENTRIES_SPACE);
                get_dir_entries(space, GET_DIR_ENTRIES_SPACE);
                for(int i = 0; i < FILES_PER_DIR; i++) {
                    if (strlen(space[i].name) > 0 && space[i].name[0] != '.')
                        println("%s " RED "%d" WHITE " %s", space[i].name, space[i].inode, 
                                space[i].type == CREATE_DIR ? BLUE "DIR" WHITE 
                                : GREEN "FILE" WHITE);
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
    printf("Hello world!\n");
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
 //   mem_init(10);

    readline_lib("BC> ", bc_parse);
    return 0;
}

int dummy_main() {
    for(;;) {
        yield();
    }
    return 0;
}
#endif
