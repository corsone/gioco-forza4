#include "../inc/shm.h"
#include "../inc/errExit.h"

int create_shm(int shmKey, size_t size, int flag){
    int shmId = shmget(shmKey, size, flag);
    if(shmId == -1)
        errExit("shmget failed");

    return shmId;
}

void *attach_shm(int shmId){
    void *ptr = shmat(shmId, NULL, 0);
    if (ptr == (void *)-1)
        errExit("shmat failed");

    return ptr;
}

void detach_shm(const void *ptr){
    if(shmdt(ptr) == -1)
        errExit("shmdt failed");
}

void remove_shm(int shmId){
    if (shmctl(shmId, IPC_RMID, NULL) == -1)
        errExit("shmctl failed");
}