#include "defs.h"

/**
 * @param c character to print to screen
 * @param at position in write-to-buffer to to c
 * @param buf the write to buffer
 * @return 0 on success, -1 on error
 * -- currently is O(n) for each char
*/
int putcat(char c, int at, struct list * buf)
{
    if (at < 0 || at > buf->len) at = buf->len;
        buf->data = realloc(buf->data, buf->len + 2);
    printf("COPY\r\n");
    memmove(&buf->data[at + 1], &buf->data[at], buf->len - at + 1);
    /**
     * @note 
         store pointers to location in LL, update O(1)
         makes size double
            extra bookkeeping to ensure accuracy
        why have buffer if we just write every loop?
    */

    buf->len++;
    // editorcfg.ccol++;
    // if(editorcfg.ccol > editorcfg.col){
    //     editorcfg.col++;
    printf("insrt\r\n");
    buf->data[at] = c;
    // }
    // editorUpdateRow(buf);
    return 0;
}

void editorUpdateRow(struct list* buff)
{
    printf("editorUpdateRow, what do?\n");
}

/**
 * @brief writes the buffer to screen
 * 
 * @param buf current buffer of changes made
 * @return int number of bytes writen
 */
int buf_write(struct list * buf)
{
    return -1;
}