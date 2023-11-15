#ifndef SERVER_H
#define SERVER_H

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

extern int rows, cols; // rows, cols of matrix
extern char *game; // matrix for the game field
extern char game_pieces[2]; // game pieces of players
extern int shmId, semId; // share memory id, semaphore set id
extern key_t semKey; // semaphore set key
extern int fifoFd; // fifo file descriptor
extern struct fifo_server2client fifo_server2client; // message from server to client
extern struct fifo_client2server fifo_client2server, fifo_client2server2; // message from client1, client2 to server
extern int counter_ctrlc; // counter for CTRL-C

/**
 * @brief check argument
 * 
 * @param argc 
 * @param argv 
 */
void check_arg(int argc, char *argv[]);

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
 * @brief create share memory, attach share memory, initialize the game field
 * 
 */
void init_shm();

/**
 * @brief write on fifoS2C
 * 
 * @param game_piece 
 * @param game_piece_opp 
 * @param n_player 
 * @param bot 
 */
void write_fifoS2C(char game_piece, char game_piece_opp, int n_player, bool bot);

/**
 * @brief read from fifoC2S
 * 
 * @param fifo_client2server 
 */
void read_fifoC2S(struct fifo_client2server *fifo_client2server);

/**
 * @brief check if a player win
 * 
 * @return true: a player win
 * @return false: nobody won
 */
bool win();

/**
 * @brief check if the match ended in a draw
 * 
 * @return true: the match ended in a draw
 * @return false: the game is not over yet
 */
bool draw();

/**
 * @brief 
 * 
 * @param turn player's turn
 * @param o first player's game piece
 * @param x second player's game piece
 * @param player who's playing
 */
void play(char turn, char o, char *player);

/**
 * @brief close IPCS 
 * 
 */
void end_game();

#endif