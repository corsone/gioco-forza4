#ifndef CLIENT_H
#define CLIENT_H

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

#include "errExit.h"
#include "sem.h"
#include "shm.h"
#include "fifo.h"

extern char *game; // matrix for the game field
extern int shmId, semId; // share memory id, semaphore set id
extern key_t semKey; // semaphore set key
extern int fifoFd; // fifo file descriptor
extern struct fifo_server2client fifo_server2client;  // message from server to client
extern struct fifo_client2server fifo_client2server;   // message from client to server

/**
 * @brief block some signal, set mask, set signal handler
 * 
 */
void init_signals();

/**
 * @brief signal handler
 * 
 * @param sig 
 */
void sigHandler(int sig);

/**
 * @brief create and initialize semaphore set
 * 
 */
void init_semaphore();

/**
 * @brief read from fifoS2C
 * 
 * @param fifo_client2server 
 */
void read_fifoS2C();

/**
 * @brief write on fifoS2C
 * 
 */
void write_fifoC2S();

/**
 * @brief attach share memory
 * 
 */
void init_shm();

/**
 * @brief print a message
 * 
 * @param string message to print
 */
void print(char *string);

/**
 * @brief check if the move is valid then place the game piece in the playing field
 * 
 * @param turn game piece of player
 */
void play(char turn);

/**
 * @brief print game field
 * 
 */
void print_game();

#endif