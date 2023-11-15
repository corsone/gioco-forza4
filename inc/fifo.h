#ifndef FIFO_H
#define FIFO_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdbool.h>

/**
 * @brief message from server to client
 * 
 */
struct fifo_server2client{
    int idServer;
    char game_piece;
    char game_piece_opp;
    int rows;
    int cols;
    int n_player;
    bool bot;
};

/**
 * @brief message from client to server
 * 
 */
struct fifo_client2server {
    int idClient;
    char player[50];
    bool opp;
};

/**
 * @brief Create a fifo
 * 
 * @param fname fifo file name
 */
void create_fifo(char *fname);

/**
 * @brief open the fifo
 * 
 * @param fname fifo file name
 * @param flag opening mode
 * @return int -> file descriptor
 */
int open_fifo(char *fname, int flag);

/**
 * @brief write on the fifo
 * 
 * @param fifoFd fifo file descriptor
 * @param buf buffer
 * @param size byte to write
 */
void write_fifo(int fifoFd, void *buf, size_t size);

/**
 * @brief read from the fifo
 * 
 * @param fifoFd fifo file descriptor
 * @param buf buffer 
 * @param size byte to read
 */
void read_fifo(int fifoFd, void *buf, size_t size);

/**
 * @brief remove the fifo
 * 
 * @param fname fifo file name
 */
void remove_fifo(char *fname);

#endif