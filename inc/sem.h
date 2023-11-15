#ifndef SEM_H
#define SEM_H

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

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/**
 * @brief Create a semaphore set
 * 
 * @param semKey semaphore set key
 * @param num number of semaphore in the set
 * @param flag semaphore set flag
 * @return int -> semaphore set id
 */
int create_sem(int semKey, int num, int flag);

/**
 * @brief initialize semaphore set
 * 
 * @param semId semaphore set id
 * @param arg union semun
 */
void set_sem(int semId, union semun arg);

/**
 * @brief 
 * 
 * @param semid semaphore set id
 * @param sem_num semaphore number in the set
 * @param sem_op operation to do
 */
void semOp(int semid, unsigned short sem_num, short sem_op);

/**
 * @brief remove the semaphore set
 * 
 * @param semId semaphore set id
 */
void remove_sem(int semId);


#endif