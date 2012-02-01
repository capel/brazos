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

#include "parse_dir.h"
#include "variant.h"

void exit(void) {
    _exit();
}

#define ARG(x) v->data[(x)]
#define IARG(x) atoi(v->data[(x)])

void perror(int error) {
  int newline = message("\n", 2);

  int stdio = lookup("~stdio");
  sink(error, stdio);
  sink(newline, stdio);
}


void print_dir(int rid) {
  char s[4096];
  view(rid, s, 4096);
  parsed_dir* d = parse_dir(s);
  if (!d) {
    println("Dir parse error: %s", s);
    return;
  }
  if (d->size == 0) {
    return;
  }
  for(size_t i = 0; i < d->size; i++) {
    if (i && i % 3 == 0) printf("\n");
    
    const char * n = d->entries[i]->name;
    switch (d->entries[i]->type) {
      case 'F':
        printf("%s%s%s  ", RED, n, WHITE);
        break;
      case 'M':
        printf("%s%s%s  ", YELLOW, n, WHITE);
        break;
      case 'D':
        printf("%s%s%s  ", GREEN, n, WHITE);
        break;
      case 'U':
        printf("%s%s%s  ", YELLOW, n, WHITE);
        break;
      case 'S':
        printf("%s%s%s  ", CYAN, n, WHITE);
        break;
      case 'N':
        printf("%s%s%s  ", RED, n, WHITE);
        break;
      default:
        println("lol wat %c", d->entries[i]->type);
        printf("?%s  ", n);
        break;
    }
  }
  printf("\n");
  cleanup_parsed_dir(d);
}

bool parse_line(char* line) {
    if (strlen(line) == 0)
        return false;

    variant vv = parse(line);
    println("%v", vv);
    //println("%v", parse(serialize(vv).s));
    dec(vv);
    return true;

    vector * v = ksplit_to_vector(line, " ");
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
                char* msg = line + strlen("echo") + 1;
                println("%s", msg);
                goto cleanup;
            }
            if (strcmp(v->data[0], "echor") == 0) {
                char* msg = line + strlen("echo") + 1;
                int rid = message(msg, strlen(msg));
                println("%d", rid);
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
            if (strcmp(v->data[0], "dredge") == 0) {
              if (v->size < 2) {
                println("dredge: dredge <fountain>");
                goto cleanup;
              }

              int rid = lookup(ARG(1));
              if (type(rid) != KO_FOUNTAIN) {
                println("Not a fountain.");
                close(rid);
                goto cleanup;
              }
              int ret = dredge(rid);
              close(rid);
              if (ret < 0) {
                perror(ret);
              }
              println("%d", ret);
              goto cleanup;
            }
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
        case 'r':
            if (strcmp(v->data[0], "rm") == 0) {
                if (v->size < 2) {
                    println("rm: rm <file>");
                    goto cleanup;
                }

                int rid = lookup("~cwd");
                int ret = unlink(rid, v->data[1]);
                if (ret < 0) {
                    perror(ret);
                }
                goto cleanup;
            }
            goto unknown;
       case 'c':
            if (strcmp(v->data[0], "cd") == 0) {
                int rid;
                if (v->size == 1) {
                  rid = lookup("/"); 
                } else { 
                  rid = lookup(ARG(1));
                  if (rid < 0) {
                    println("Bad directory");
                    goto cleanup;
                  }
                  if (type(rid) != KO_DIR) {
                    println("Not a directory");
                    goto cleanup;
                  }
                }

                int cwd = lookup("~");
                link(cwd, rid, "cwd");
                goto cleanup;
            } else if (strcmp(v->data[0], "cat") == 0) {
              int rid = lookup(v->data[1]);
              if (rid < 0) {
                println("Bad filename");
                goto cleanup;
              }
              char s[4096];
              view(rid, s, 4096);
              println("%s", s);
              goto cleanup;
            }
            goto unknown;
            
        case 's':
            if (strcmp(v->data[0], "sink") == 0) {
              int src_rid = lookup(ARG(1));
              debug("src %d", src_rid);
              if (src_rid < 0) {
                println("Bad file %s", ARG(1));
                goto cleanup;
              }

              int sh = lookup(ARG(2));
              debug("sh %d", sh);
              if (sh < 0) {
                println("Bad file %s", ARG(2));
                goto cleanup;
              }
              int ret = sink(src_rid, sh);
              if (ret == SINK_ASYNC) {
                goto cleanup;
              } else if (ret < 0) {
                perror(ret);
                goto cleanup;
              } else {
                println("Rid %d", ret);
              goto cleanup;
              }
            }
            if (strcmp(v->data[0], "sinkr") == 0) {
              int src_rid = IARG(1);
              debug("src %d", src_rid);
              if (src_rid <= 0) {
                println("Bad rid %s", ARG(1));
                goto cleanup;
              }

              int sh = lookup(ARG(2));
              debug("sh %d", sh);
              if (sh < 0) {
                println("Bad file %s", ARG(2));
                goto cleanup;
              }
              int ret = sink(src_rid, sh);
              if (ret == SINK_ASYNC) {
                goto cleanup;
              } else if (ret < 0) {
                perror(ret);
                goto cleanup;
              } else {
                println("Rid %d", ret);
              goto cleanup;
              }
            }
            goto unknown;
        case 'l':
            if (strcmp(v->data[0], "lookup") == 0 || strcmp(v->data[0], "l") == 0) {
              int rid = lookup(v->data[1]);
              println("Rid: %d", rid);
              goto cleanup;
            }
            if (strcmp(v->data[0], "link") == 0) {
              if (v->size < 3) {
                println("Link <src> <dst>");
              }

              char * dst = ARG(2);
              size_t dst_len = strlen(dst);
              if (dst[dst_len] == '/') {
                println("Bad filename %s (you can't end in /)", dst);
                goto cleanup;
              }
              
              bool abs = false;
              if (dst[0] == '/') { // abs
                abs = true;
                dst++;
              }

              bool found = false;
              char * name = dst;
              for(int i = dst_len; i > 0; i--) {
                if (dst[i] == '/') {
                  dst[i] = '\0';
                  name = dst + i+1;
                  found = true;
                  break;
                }
              }

              debug("child %s : parent %s : name %s", ARG(1), found ? "cwd" : dst, name);

              int prid;
              if (found) {
                prid = lookup(dst);
              } else {
                prid = lookup(abs ? "/" : "~cwd");
              }

              if (prid < 0) println("Bad parent");
              int crid = lookup(ARG(1));
              if (crid < 0) println("Bad child");
              link(prid, crid, name);

              goto cleanup;
            }
            if (strcmp(v->data[0], "ls") == 0) {
              int rid;
              if (v->size == 1) {
                rid = lookup("~cwd");
              } else {
                rid = lookup(v->data[1]);
              }
              if (rid < 0) {
                println("Bad filename");
                goto cleanup;
              }
              if (type(rid) == KO_DIR) {
                print_dir(rid);
              } else {
                println("Not a directory.");
              }
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

void kputs(const char* s);

void kprintf(const char* s, ...);
void* cp(void);
int sh_main()
{
    printf("Hello world!\n");
    readline_lib("brazos> ", parse_line);
    return 0;
}
    


bool bc_parse(char* line) {
    vector * v = ksplit_to_vector(line, " ");
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
