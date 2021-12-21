#ifndef _DEFS_
#define _DEFS_
#include "UIUX.h"

#ifndef _INCL_
#define _INCL_
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "editor.h"
#endif

struct list {
    char* data;
    int n;
    int len;
};

struct edcfg{
    struct termios default_termios;
    int total_rows; // total rows
    int col;    // total columns
    int crow;   //cursor row
    int ccol;   //cursor column
    int index;
    int len;
    int* char_per_line;
    char* fileconts;
}editorcfg;

#define max(a,b) (a>b)? a:b

/**
 * @brief Appends to the end of a linked list, auto increments length
 *
 * @param buff the linked list to be appended to
 * @param s the new data to be appended
 * @param the length of s
 * 
 * @return 0 on success, -1 of failure
*/
int append(struct list *buff, const char* s, int len)
{
    char *newm;
    if(buff->len + len > buff->n)
    {
        newm = realloc(buff->data,buff->len+len);
        if( newm == NULL) return -1;
    }
    memcpy(&newm[buff->len],s,len);
    buff->data = newm;
    buff->n += len;
    buff->len+=len;
    return 0;
}
/**
 * @brief frees the data in the linked list
 * 
 * @param a pointer to the linked list to be freed
 */
void llfree(struct list* a)
{
    free(a->data);
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
int putcat(char c, int at);
int remcat(int at);
void mcusorleft(int x);
void mcusorright(int x);

void showbuff();
int original_fd;
struct {
    int size;
    char* buffer;
} temp_file;


#endif