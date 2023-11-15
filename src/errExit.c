#include "../inc/errExit.h"

void errExit(char *msg){
    perror(msg);
    exit(1);
}