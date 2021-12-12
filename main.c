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
    int r;
    int co; 
    switch (c)
    {
        case '!':
            exit(0);
        break;
        case '?':
        if(cursorpos(&r,&co) != 0)
        {
           break;
        }
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
        //    putcat('!',editorcfg.ccol+1,&writetobuf);
        // writetobuf.data[0] = ' ';
        // printf("buff == %s\n",writetobuf.data);
        // exit(1);
            // do i have to re write this line?
        break;
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
    writetobuf.data = malloc(sizeof(char)*2);
    writetobuf.n = 2;
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
    temp_file.size = size+2;
    temp_file.buffer = (char*)malloc((sizeof(char)* size) + 2);
    if(read(fd,temp_file.buffer,size) == -1){
        printf("error copying file to ram errno: %d\n",errno);
        rawmodedel();
        exit(1);
    }
    temp_file.buffer[size] = '\n';
    temp_file.buffer[size+1] = '\r';
    close(fd);
    //
    int i = 0;
    char c;
    int rowcount = 0;
    for(;i < temp_file.size+1; ++i){
        c= temp_file.buffer[i];
        if(c == '\n'){
           rowcount++;
        }
    }
    int row = 0;
    editorcfg.fileconts = (struct ll*)malloc(sizeof(struct ll) * rowcount);// alloc once, instead of many times in loop, may have to alloc more, as use edits
    for(i=0;i < temp_file.size+1; ++i){
        c= temp_file.buffer[i];
        if(c == '\n'){
            row++;
            continue;
        }
        append(&editorcfg.fileconts[row],&c,1);
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
    while(1)
    {
        handlekey(readkey());
        buffwrite();
    }
    free(temp_file.buffer);
    return 0;
}