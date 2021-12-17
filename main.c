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

void copytotemp(int fd,int size){
   editorcfg.fileconts = (char*) malloc(sizeof(char) * size);
    read(fd,editorcfg.fileconts,size);
    editorcfg.char_per_line = (int*)malloc(sizeof(int)*size);
    int i = 0;
    int col = 0;
    int row = 0;
    for(i; i < size; ++i){
        col++;
        if(editorcfg.fileconts[i] == '\n'){
            editorcfg.char_per_line[row] = col;
            row++;
            col = 0;
        }
    }
}

/**
 * return terminal to original mode
*/
void rawmodedel(void)
{
    free(editorcfg.char_per_line);
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
    int r;
    int co; 
    if(cursorpos(&editorcfg.crow,&editorcfg.ccol) != 0)
    {
        return;
    }
    printf("at %d\r\n",editorcfg.crow);
    if (editorcfg.crow > editorcfg.row){
        exit(1);
        editorcfg.crow = editorcfg.row-1;
    }
    if (editorcfg.ccol > editorcfg.col){
       editorcfg.ccol =0;
    }

    switch (c)
    {
        case '!':
            exit(0);
        break;
        case '?':
        
        if(r == 0)
            r++;
        if(co == 0)
            co++;
        printf("%c\r\n",editorcfg.fileconts[r-1].data[co-1]);
        break;
        case BACKSPACE: // debug, never actually gets here
            if(cursorpos(&r,&co) != 0)
            {
                break;
            }
            if(r == 1)
                r++;
            if(co == 1)
                co++;
            editorcfg.fileconts[r-1].data[co-1] = ' ';
            write(STDOUT_FILENO," ",1);
            if(editorcfg.ccol != 0)
                editorcfg.ccol--;
        break;
        default:
        printf("DEFAULT\r\n");
        printf("at %d\r\n",editorcfg.crow);
            if(c == '\n')
            {
                putcat('\r',editorcfg.ccol-1,&editorcfg.fileconts[editorcfg.crow-1]);
            }
            printf("cur %d,%d, max %d,%d\r\n",editorcfg.ccol,editorcfg.crow,editorcfg.col,editorcfg.row);
            printf("len: %d\r\n",editorcfg.fileconts[0].len);

            printf("PUTCAT\r\n");
            putcat('\r',editorcfg.ccol-1,&editorcfg.fileconts[editorcfg.crow-1]);
            editorcfg.crow++;
            if(editorcfg.row  == editorcfg.crow){
                // TODO : append to filconts
              exit(1);
            }
            editorcfg.ccol = 0;
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

}

/**
 * 
*/
int main(int argc, char** argv)
{
    original_fd = -1;
    struct stat st;
    if(argc == 2){
        original_fd = open(argv[1], O_RDWR | O_CREAT | O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRUSR );
        if(fstat(original_fd,&st) == -1){
            printf("error opening file %s\n", argv[1]);
            return 0;
        }
        if(st.st_mode  ==  S_IFDIR ){
            printf("will not open a directory\n");
            return 0;
        }
        copytotemp(original_fd,(int)(long)st.st_size);
    }
    rawmodeinit();
    // bufferinit();
    editoruiinit();
    while(1)
    {
        handlekey(readkey());
        buffwrite();
    }
    free(temp_file.buffer);
    return 0;
}