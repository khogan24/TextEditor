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
#include "editor.h"

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
#define BACKSPACE 127
void handlekey(const char c)
{
    switch (c)
    {
        case '!':
            exit(0);
        break;
        case UPARROW:
            if(editorcfg.crow != 0){
            editorcfg.crow--;
            }
        break;
        case DOWNARROW:
            if(editorcfg.crow < editorcfg.rows-1){
                editorcfg.crow--;
            }
        break;
        case LEFTARROW:
            if(editorcfg.ccol == 0){// go up
                editorcfg.crow--;
                if(editorcfg.fileconts[editorcfg.crow].len == 0){
                    putcat(' ',0);
                }
                editorcfg.ccol = editorcfg.fileconts[editorcfg.crow].len-1;
            }
        break;
        case RIGHTARROW:
            if(editorcfg.ccol == editorcfg.fileconts[editorcfg.crow].len-1){// go down
                if(editorcfg.crow > editorcfg.rows)
                    editorcfg.crow++;
                editorcfg.ccol = 0;
            }
        break;
        default:
        printf("DEFAULT\r\n");
            if(c == '\n')
            {
                putcat('\r',editorcfg.ccol);
                editorcfg.ccol = 0;
                editorcfg.crow++;
               //print /r/n
            }
            putcat(c,editorcfg.ccol);

          //print

    }
}

void bufferinit(void)
{
;
}

/**
 * @brief writes the buffer to the screen,
 *        clearing the buffer when done
 */
void buffwrite()
{
    printf("WRITE:");
    char* str = editorcfg.fileconts[editorcfg.crow].data;
    int crow = editorcfg.crow;
    int ccol = editorcfg.ccol;
    int len = editorcfg.fileconts[crow].len - ccol - 1; // -1 for 0 index array

    write(STDOUT_FILENO,str + ccol,len);
}

/**
 * @brief copies the contents of the file pointed to by fd into the in-memory buffer.
 * This is to allow fordiscarding of edits
 * 
 * @param fd file descriptor of the file opened
 */
// there is a smarter way, to use a swp file and only load visible and near visible chars, but eh
// what if i mmap lazily?
void copytotemp(int fd, int size){
    char file[size];
    if(read(fd,file,size) != size){
        printf("There was an error when reading file from disk");
        exit(1);
    }
    char buffer[BUFFER_SIZE];
    int i = 0;
    char c;
    int rowc = 0;
    int colc = 0;
    for(; i < size; ++i){
        c = file[i];
        buffer[colc] = c;
        colc++;
        if(c == '\n'){
            editorcfg.fileconts[rowc].data = malloc(sizeof(char) * colc);
            memcpy(editorcfg.fileconts[rowc].data,buffer,colc);
            editorcfg.fileconts[rowc].len = colc;
            editorcfg.fileconts[rowc].n = colc;
            colc = 0;
            rowc++;
            editorcfg.rows++;
        }
    }
}

/**
 * 
*/
int main(int argc, char** argv)
{
    original_fd = -1;
    struct stat fileinf;
    if(argc == 2){
        original_fd = open(argv[1], O_RDWR | O_CREAT | O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRUSR );
        if(fstat(original_fd,&fileinf) == -1){
            printf("error opening file %s\n", argv[1]);
            return 0;
        }
        if(fileinf.st_mode  ==  S_IFDIR ){
            printf("will not open a directory\n");
            return 0;
        }
        copytotemp(original_fd,(int)(long)fileinf.st_size);
    }
    rawmodeinit();
    bufferinit();
    editoruiinit();
    // i need only ping once, at start, keep track of key presses
    if(cursorpos(&editorcfg.crow,&editorcfg.ccol) != 0)
    {
        printf("There was an error with the cursor\r\n");
        return 1;
    }
    while(1)
    {
        handlekey(readkey());
        buffwrite();
    }
    free(temp_file.buffer);
    return 0;
}