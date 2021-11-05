#ifndef _DEFS_
#define _DEFS_
#include "UIUX.h"

#ifndef _INCL_
#define _INCL_
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
#endif

struct{
    struct termios default_termios;
    int row;
    int col;
}editorcfg;

#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

/**
 * Gets the index of target in an array
 * @param target the element we are searching for
 * @param buf the array to look through
 * @returns index of target in buffer, -1 if not found
*/
const int indexof(const char target,const char* buf)
{
    int i = 0;
    while(buf[i] != '\n')
    {
        if(buf[i] == target) 
            return target;
        ++i;
    }
    return -1;
    
}

#define CTRL_KEY(k) (((char)k) & 0x1f)

const void panic(const char*);
int windowsize(int*, int*);
int cursorpos(int*, int *);
void editoruiinit(void);
void clearterm(void);

#endif