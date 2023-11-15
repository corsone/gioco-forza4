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
#include <time.h>

#include "../inc/errExit.h"
#include "../inc/sem.h"
#include "../inc/shm.h"
#include "../inc/fifo.h"
#include "../inc/client.h"


int main(int argc, char *argv[]){
    /*
     check arg
    */
    if(argc != 2 && argc != 3){
        printf("Usage: %s <user name> <optional: *>\n", argv[0]);
        exit(0);
    }

    /*
     handle signals
    */
    init_signals();

    /*
     semaphore set
    */
    init_semaphore();

    semOp(semId, 1, 0);

    /*
     fifo server->client in lettura
    */
    read_fifoS2C();

    /*
     fifo client->server in scrittura
    */
    create_fifo("fifoC2S");

    semOp(semId, 0, 1);

    fifo_client2server.idClient = getpid();
    strcpy(fifo_client2server.player, argv[1]);
    fifo_client2server.opp = (argc == 2)? true : false;
    write_fifoC2S();

    /*
     share memory
    */
    init_shm();


    if (!fifo_server2client.bot){ // if player is not a bot
        printf("\n<Client> game is starting...\n");

        if(fifo_server2client.n_player == 0){
            printf("<Client> waiting for another player...\n");
            semOp(semId, 1, -1); 
        }else{
            printf("<Client> wait, it's your opponent's turn...\n");
            semOp(semId, 2, -1); 
        }

        printf("\n<Client> your game piece is %c, your opponent's game piece is %c\n", fifo_server2client.game_piece, fifo_server2client.game_piece_opp);

        while (1){
            semOp(semId, 3, -1);
            
            print_game();
            alarm(30);
            play(fifo_server2client.game_piece);
            alarm(0);
            print_game();

            semOp(semId, 3, 1);
            semOp(semId, 0, 1); 
            
            printf("<Client> wait, it's your opponent's turn...\n");
            if(fifo_server2client.n_player == 0)
                semOp(semId, 1, -1);
            else
                semOp(semId, 2, -1);
        }
    }else if(fifo_server2client.bot) { // if player is a bot
        semOp(semId, 2, -1); 
        srand(time(NULL));

        while (1){
            sleep(1);
            
            semOp(semId, 3, -1);
            alarm(30);
            play(fifo_server2client.game_piece);
            alarm(0);
            semOp(semId, 3, 1);
            semOp(semId, 0, 1);
            
            semOp(semId, 2, -1);
        }
    }
   
    return 0;
}