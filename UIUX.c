#include "defs.h"
/**
 * clears the terminal
 */
void clearterm(void)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void editoruiinit(void)
{
    clearterm();
    if(windowsize(&(editorcfg.row),&(editorcfg.col)) !=0)
    {
        panic("ERR: winsize");
    }
    int i = 1;
    for(;i < editorcfg.row; ++i)
        write(STDOUT_FILENO,"-\r\n",3);
    write(STDOUT_FILENO,"-",1);
}

/**
 * Gets the position of the cursor, (0,0) is top left
 * @param r an empty buffer to store the row pos
 * @param c an empty buffer to store the col pos
 * @return 0 on success with the r and c buffers populated
 *        -1 on failure with r and c set to -1
 * @note further reading : https://vt100.net/docs/vt100-ug/chapter3.html#CPR
*/
int cursorpos(int* r, int *c)
{   
    *r = -1;
    *c = -1;
    return -1;
    if(write(STDOUT_FILENO,"\x1b[6n", 4) != 4)
    {
        *r = -1;
        *c = -1;
        return -1;
    }
    char buffer[32];
    // printf("\r\n");
    int i = 0;

    while(i < NELEM(buffer)-1)
    {
        if(read(STDIN_FILENO,&buffer[i],1) != -1)break;
        if(buffer[i] == 'R') break;
        ++i;
    }
    buffer[i] = '\0';
    int ind;
    if(i <= 0 || (ind= indexof(';',&buffer[2])) == -1  || buffer[0] != '\x1b' || buffer[1] != '[')
    {
        *r = -1;
        *c = -1;
        return -1;
    }
    
    if(sscanf(&buffer[2],"%d;%d",r,c) != 2)
        return -1;

    return 0;
}
/**
 * Gets the size of the window
 * @param r buffer for window row count
 * @param c buffer for window column count
 * @return 0 on success with row and column buffers populated
 *         -1 of error with r and c both as -1;
*/
int windowsize(int *r, int *c)
{
    struct winsize w;
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ, &w) == -1 || w.ws_col == 0)
    {
        *r = -1;
        *c = -1;
        return -1;
    }
    *r = w.ws_row;
    *c = w.ws_col;
    return 0;
}