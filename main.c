#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "defs.h"

// struct termios original_term_settings;

/**
 * return terminal to original mode
*/
void rawmodedel(void)
{
    clearterm();
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&editorcfg.default_termios);
}

void panic(const char* str)
{
    clearterm();
    printf("panic: %s", str);
    exit(1);
}

/**
 * Move from 'cooked' mode to raw.
 *  dont want to press 'enter' for every letter
*/
void rawmodeinit(void)
{
    if(tcgetattr(STDIN_FILENO, & editorcfg.default_termios) != 0)
    {
        printf("ERR: could not set up raw mode\b");
        tcsetattr(STDIN_FILENO,TCSAFLUSH,&editorcfg.default_termios);
        exit(0);
    }
    atexit(rawmodedel);

    struct termios r = editorcfg.default_termios;
    r.c_iflag &= ~(IXON|IXON|BRKINT|ISTRIP);
    r.c_oflag &= ~(OPOST);
    r.c_cflag |= (CS8);
    r.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);

    r.c_cc[VMIN] = 0;
    r.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO,TCSAFLUSH,&r);
}

/**
 * wait for input, return input
 *  fails if error
*/ 
char readkey(void)
{
    int n;
    char c;
    while((n = read(STDIN_FILENO,&c,1)) != 1)
    {
        if(n == -1 && errno != EAGAIN)
        {
            panic("readkey");
        }
    }
    return c;
}

void handlekey(const char c)
{
    printf("handle\r\n");
    switch (c)
    {
        case '!':
            exit(0);
        break;
        
        default:
           write(STDOUT_FILENO,&c,1);
           if(c == '\n')
            write(STDOUT_FILENO,"\r",1);
    }
}

/**
 * 
*/
int main(void)
{
    rawmodeinit();
    editoruiinit();
    while(1)
    {
        handlekey(readkey());
    }
    return 0;
}