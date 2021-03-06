#define USER


#include "user.h"
#include "user_syscalls.h"
#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"
#include "vector.h"

void erase_chars(size_t size);


void readline_lib(const char * prompt, readline_func func) {
  char buf[4096];
  int stdin = lookup("/stdin");
  println("stdin %d", stdin);

  int rid;
  for (;;) {
    printf("%s", prompt);
    while ((rid = dredge(stdin)) == 0);
    view(rid, buf, 4096);
    func(buf);
  }
}

/*



    vector * history = kmake_vector(sizeof(char*), MANAGED_POINTERS);
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

        // TODO;
        int reason = line_offset; // getline(line + line_offset, 100);
       

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
                    erase_chars(strlen(line));
                    history_pos--;
                    line = history->data[history_pos];
                    printf("%s", line);
                    fresh_line = false;
                    line_offset = strlen(line);
                }
                break;
            case ARROW_DOWN:
                if (history_pos == history->size) {
                    fresh_line = false;
                    line_offset = 0;
                } else if (history_pos == history->size -1) {
                    if (fresh_line) {
                        fresh_line = false;
                        line_offset = strlen(line);
                    } else {
                        fresh_line = false;
                        erase_chars(strlen(line));
                        history_pos = history->size - 1;
                        line_offset = 0;
                    }  
                } else {
                    erase_chars(strlen(line));
                    history_pos++;
                    line = history->data[history_pos];
                    printf("%s", line);
                    fresh_line = false;
                    line_offset = strlen(line);
                }
                break;
            case NEWLINE:
                fresh_line = true;
                len = strlen(line);
                line[len-1] = '\0';
                bool ret = func(line);
                if (ret) { // save to history
                    // ignore dup lines right next to each other
                    //if (0 == strcmp(history->data[history_pos], line))
                    //    break;
                    vector_push(history, line);
                    history_pos = history->size;
                }
                
            default:
                break;
        }
    }
}
*/

void close(int rid) {
  int rids = lookup("~rids");
  char n[32];
  itoa(n, 32, rid);
  unlink(rids, n);
}


