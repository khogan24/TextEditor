#include "defs.h"

/**
 * @param c character to print to screen
 * @param at position in write-to-buffer to to c
 * @param buf the write to buffer
 * @return 0 on success, -1 on error
 * -- currently is O(n) for each char
*/
int putcat(char c, int at)
{
    if(at < 0){
        return -1;
    }
    if(at > editorcfg.len){
        //grow
        char* temp = (char*)malloc(sizeof(char)* at);
        memset(temp,'\0',at);
        memmove(temp,editorcfg.fileconts,editorcfg.len);
        free(editorcfg.fileconts);
        editorcfg.fileconts = temp;
        editorcfg.len = at;
    }
    char* temp = (char*)malloc(sizeof(char) * editorcfg.len+1);
    memcpy(temp,editorcfg.fileconts,editorcfg.index);
    temp[editorcfg.index] = c;
    memcpy(temp + editorcfg.index+1,editorcfg.fileconts + editorcfg.index,editorcfg.len - editorcfg.index);
    free(editorcfg.fileconts);
    editorcfg.fileconts = temp;
    editorcfg.len++;
    return 0;
}

/**
 * @brief removes a char at index, reduces size of buffer
 * 
 * @param at index of char to be removed
 * 
 */
int remcat(int at){
    if(at < 0 || at > editorcfg.len){
        return -1;
    }

    char* temp = (char*)malloc(sizeof(char)*editorcfg.len-1);
    memcpy(temp,editorcfg.fileconts,at);
    memcpy(temp+at, editorcfg.fileconts + at, editorcfg.len - at);
    free(editorcfg.fileconts);
    editorcfg.fileconts = temp;
    editorcfg.len--;
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