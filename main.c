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
#define BACKSPACE 0x08
void handlekey(const char c)
{
    switch (c)
    {
        case '!':
            exit(0);
        break;
        case BACKSPACE: // debug, never actually gets here
        printf("ahahhahahahhaaha");
        while(1)
            ;
        
        default:
            if(c == '\n')
            {
                putcat('\r',editorcfg.ccol,&writetobuf);
            }
            putcat(c,editorcfg.ccol,&writetobuf);
    }
}

void bufferinit(void)
{
    writetobuf.data = malloc(sizeof(char)*512);
    writetobuf.n = 512;
    writetobuf.len = 0;
}

/**
 * @brief writes the buffer to the screen,
 *        clearing the buffer when done
 */
void buffwrite()
{
    write(STDOUT_FILENO,writetobuf.data,writetobuf.len);
    // llfree(&appendbuf);// do i need free//
    writetobuf.len = 0;
    writetobuf.n = 0;
    // appendbuf.data;
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
    temp_file.size = size;
    temp_file.buffer = (char*)malloc((sizeof(char) * temp_file.size) +2);
    if(read(fd,temp_file.buffer,size) == -1){
        printf("error copying file to ram errno: %d\n",errno);
        free(temp_file.buffer);
        rawmodedel();
        exit(1);
    }
    close(fd);
    temp_file.buffer[size+1] = '\r';
    temp_file.buffer[size] = '\n';
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
    while(1)
    {
        handlekey(readkey());
        buffwrite();
    }
    free(temp_file.buffer);
    return 0;
}