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
#include <string.h>
#endif

struct{
    struct termios default_termios;
    int row;
    int col;
}editorcfg;

struct ll {
    char* b;
    int n;
    int len;
} appendbuf;

#define max(a,b) (a>b)? a:b

/**
 * Appends to the end of a linked list
 *  appends at leaset 512 bytes more than was needed, and only appends if the current write is long enough
 * @param buff the linked list to be appended to
 * @param s the new data to be appended
 * @param the length of s
 * @return 0 on success, -1 of failure
*/
int append(struct ll *buff, const char* s, int len)
{
    char *newm;
    int nlen;
    if(buff->len + len > buff->n)
    {
        nlen = max(512, len);
        newm = realloc(buff->b,buff->len + nlen + 512);
        if( newm == NULL) return -1;
    }
    memcpy(&newm[buff->len],s,nlen);
    buff->b = newm;
    buff->n += nlen;
    buff->len+=nlen;
    return 0;
}

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