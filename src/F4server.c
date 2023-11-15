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

#include "../inc/errExit.h"
#include "../inc/sem.h"
#include "../inc/shm.h"
#include "../inc/fifo.h"
#include "../inc/server.h"

int main(int argc, char *argv[]){
    /*
     check arg
    */
    check_arg(argc, argv);
    
    /*
     handle signals
    */
    init_signals();

    /*
     semaphore set
    */
    init_semaphore();

    /*
     share memory
    */
    init_shm();

    printf("<Server> waiting for the first player...\n");

    /*
     fifo server->client1 in scrittura
    */
    create_fifo("fifoS2C");

    semOp(semId, 1, -1);

    write_fifoS2C(game_pieces[0], game_pieces[1], 0, false);

    semOp(semId, 0, -1);

    /*
     fifo client1->server in lettura
    */
    read_fifoC2S(&fifo_client2server);

    if(fifo_client2server.opp){
        printf("<Server> waiting for the second player...\n");
    }else{
        printf("<Server> the client play against the computer...\n");

        pid_t child = fork();
        if (child == -1)
            errExit("fork failed");
        else if(child == 0){
            char *args[] = {"F4client", "computer", NULL};
            execv("./F4client", args);
        }
    }

    /*
     fifo server->client2 in scrittura
    */
    write_fifoS2C(game_pieces[1], game_pieces[0], 1, (fifo_client2server.opp)? false : true);


    semOp(semId, 0, -1);

    /*
     fifo client2->server in lettura
    */
    read_fifoC2S(&fifo_client2server2);

    printf("\n<Server> game is starting...\n");

    char turn = game_pieces[0];
    char *player = fifo_client2server.player;

    while (1){  
        play(turn, game_pieces[0], player);
        turn = (turn == game_pieces[0])? game_pieces[1] : game_pieces[0];

        if (fifo_client2server.opp)
            player = (strcmp(player, fifo_client2server.player) == 0)? fifo_client2server2.player : fifo_client2server.player;
        else
            player = (strcmp(player, fifo_client2server.player) == 0)? "computer" : fifo_client2server.player;
    }

    return 0;
}