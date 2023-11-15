#include "../inc/sem.h"
#include "../inc/errExit.h"

int create_sem(int semKey, int num, int flag){
    int semId = semget(semKey, num, flag);
    if(semId == -1)
        errExit("semget failed");

    return semId;
}

void set_sem(int semId, union semun arg){
    if (semctl(semId, 0 /*ignored*/, SETALL, arg) == -1)
        errExit("semctl failed");
}

void semOp(int semid, unsigned short sem_num, short sem_op){
    struct sembuf sop = {
            .sem_num = sem_num,
            .sem_op = sem_op,
            .sem_flg = 0
    };

    int n;
    do{
        n = semop(semid, &sop, 1);
    }while(n == -1 && errno == EINTR);
    
    if(n == -1)
        errExit("semop failed");
}

void remove_sem(int semId){
    if (semctl(semId, 0/*ignored*/, IPC_RMID, 0/*ignored*/) == -1)
        errExit("semctl failed");
}