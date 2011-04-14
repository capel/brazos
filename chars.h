#ifndef CHARS_H
#define CHARS_H

#define ARROW_LEFT 0xFF00
#define ARROW_RIGHT 0xFF01
#define ARROW_UP 0xFF02
#define ARROW_DOWN 0xFF03

#define NEWLINE 0xFF04

#define BAD_CODE 0xFF00

#define OUT_OF_SPACE 0xFFFF


#define BLACK "\033[22;30m"
#define RED "\033[22;31m"
#define GREEN "\033[22;32m"
#define BROWN "\033[22;33m" 
#define BLUE "\033[22;34m"
#define MAGENTA "\033[22;35m" 
#define CYAN "\033[22;36m"
#define GRAY "\033[22;37m"
#define DARK_GRAY "\033[01;30m"
#define LIGHT_RED "\033[01;31m"
#define LIGHT_GREEN "\033[01;32m" 
#define YELLOW "\033[01;33m"
#define LIGHT_BLUE "\033[01;34m"
#define LIGHT_MAGENTA "\033[01;35m" 
#define LIGHT_CYAN "\033[01;36m"
#define WHITE "\033[01;37m" 

#define isspecial(c) ((c) > 255)

#endif
