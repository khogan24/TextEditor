#include "defs.h"

/**
 * @param c character to print to screen
 * @param at position in write-to-buffer to to c
 * @return 0 on success, -1 on error
 *Amertized runtime, doubles each growth
*/
int putcat(const char c, const int at)
{
    int crow = editorcfg.crow;
    if(at > editorcfg.fileconts[crow].n){
        char* temp = malloc(sizeof(char)*editorcfg.fileconts[crow].n *2);
        memcpy(temp,editorcfg.fileconts[crow].data,editorcfg.fileconts[crow].n);
        free (editorcfg.fileconts[crow].data);
        editorcfg.fileconts[crow].data = temp;
        editorcfg.fileconts[crow].n *=2;
    }
    editorcfg.fileconts[crow].data[at] = c;
    editorcfg.fileconts[crow].len++;
    return 1;
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