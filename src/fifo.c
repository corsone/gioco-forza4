#include "../inc/fifo.h"
#include "../inc/errExit.h"

void create_fifo(char *fname){
    if (mkfifo(fname, S_IRUSR | S_IWUSR) == -1)
        if(errno != EEXIST)
            errExit("mkfifo failed");
}

int open_fifo(char *fname, int flag){
    int fifoFd = open(fname, flag);
    if (fifoFd == -1)
        errExit("open failed");

    return fifoFd;
}

void write_fifo(int fifoFd, void *buf, size_t size){
    size_t bW = write(fifoFd, buf,  size);
    if(bW != size)
        errExit("fifoS2C write failed");
}

void read_fifo(int fifoFd, void *buf, size_t size){
    size_t bR = read(fifoFd, buf, size);
    if(bR <= 0 || bR != size)
        errExit("read failed");
}

void remove_fifo(char *fname){
    if(access(fname, F_OK) == 0) {
        // remove fifo
        if (unlink(fname) == -1){
            printf("unlink failed");
            exit(1);
        }
    }
}