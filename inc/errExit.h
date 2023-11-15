#ifndef ERREXT_H
#define ERREXT_H

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


/**
 * @brief describes an error and exits
 * 
 * @param msg error message
 */
void errExit(char *msg);

#endif