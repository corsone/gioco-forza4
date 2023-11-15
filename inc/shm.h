#ifndef SHM_H
#define SHM_H

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
 * @brief Create a share memory
 * 
 * @param shmKey share memory key
 * @param size share memory size
 * @param flag share memory flags
 * @return int -> share memory id
 */
int create_shm(int shmKey, size_t size, int flag);

/**
 * @brief attach to share memory
 * 
 * @param shmId share memory id
 * @return void* -> address at which shared memory is attached
 */
void *attach_shm(int shmId);

/**
 * @brief detach from share memory
 * 
 * @param ptr pointer to share memory
 */
void detach_shm(const void *ptr);

/**
 * @brief remove share memory
 * 
 * @param shmId share memory id
 */
void remove_shm(int shmId);


#endif