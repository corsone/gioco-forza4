#include "../inc/client.h"


char *game; // matrix for the game field
int shmId, semId; // share memory id, semaphore set id
key_t semKey; // semaphore set key
int fifoFd; // fifo file descriptor
struct fifo_server2client fifo_server2client;  // message from server to client
struct fifo_client2server fifo_client2server;   // message from client to server

void init_signals(){
    sigset_t sigSet;
    sigfillset(&sigSet);
    sigdelset(&sigSet, SIGINT);
    sigdelset(&sigSet, SIGQUIT);
    sigdelset(&sigSet, SIGUSR1);
    sigdelset(&sigSet, SIGUSR2);
    sigdelset(&sigSet, SIGTERM);
    sigdelset(&sigSet, SIGCONT);
    sigdelset(&sigSet, SIGALRM);

    if (sigprocmask(SIG_SETMASK, &sigSet, NULL) == -1)
        errExit("sigprocmask failed");
    if (signal(SIGINT, sigHandler) == SIG_ERR || signal(SIGQUIT, sigHandler) == SIG_ERR || signal(SIGUSR1, sigHandler) == SIG_ERR || signal(SIGUSR2, sigHandler) == SIG_ERR || signal(SIGTERM, sigHandler) == SIG_ERR || signal(SIGCONT, sigHandler) == SIG_ERR || signal(SIGALRM, sigHandler) == SIG_ERR)
        errExit("signal failed");
}

void sigHandler(int sig){ 
    switch (sig) {
        case SIGQUIT: // se il server viene chiuso
            print("<Client> the game was ended from the outside");
            exit(0);

        case SIGINT: // se un client quitta
            kill(fifo_server2client.idServer, SIGQUIT);
            exit(0);

        case SIGCONT: // se un client quitta, vince l'avversario
            print("<Client> the opponent has left the game, you have won by forfeit!");
            exit(0);

        case SIGUSR1: // se vinco
            print("<Client> Congratulations, you've won!");
            exit(0);

        case SIGUSR2: // se perdo
            print_game();
            print("<Client> Sorry, you lost!");
            exit(0);

        case SIGTERM: // se pareggio
            print("<Client> The match ended in a draw!");
            exit(0);

        case SIGALRM: // se scade il tempo
            print("<Client> the time to make the move has expired, you lost!");
            kill(getpid(), SIGINT);


            break;

        default:
            break;
    }
}

void init_semaphore(){
    semKey = ftok("F4server.c", 0);
    semId = semget(semKey, 4, S_IWUSR | S_IRUSR);
    if(semId == -1)
        errExit("semget failed");
}

void read_fifoS2C(){
    fifoFd = open_fifo("fifoS2C", O_RDONLY);
    read_fifo(fifoFd, &fifo_server2client, sizeof(struct fifo_server2client));
    close(fifoFd);
}

void write_fifoC2S(){
    fifoFd = open_fifo("fifoC2S", O_WRONLY);
    write_fifo(fifoFd, &fifo_client2server, sizeof(struct fifo_client2server));
    close(fifoFd);
}

void init_shm(){
    key_t shmKey = ftok("F4server.c", 1);
    shmId = create_shm(shmKey, sizeof(char) * fifo_server2client.rows * fifo_server2client.cols, S_IWUSR | S_IRUSR);

    game = (char *) attach_shm(shmId);
}

void print(char *string){
    int len = strlen(string);

    if (!fifo_server2client.bot){
        printf("\n\n ");
        for (int i=0;i<len + 4;i++)
            printf("*");
        
        printf("\n *");
        for (int i=0;i<len + 2;i++)
            printf(" ");

        printf("*\n");    
        printf(" * %s *\n", string);
        
        printf(" *"); 
        for (int i=0;i<len + 2;i++)
            printf(" ");
        printf("*\n ");
        for (int i=0;i<len + 4;i++)
            printf("*");
        printf("\n\n");
    }
}

void play(char turn){
    int n = 0;
    
    if (fifo_server2client.bot){
        do{
            n = rand() % fifo_server2client.cols;
        } while (n <= 0 || n>fifo_server2client.cols || game[0*fifo_server2client.cols+n-1] != ' ');
        
        for (int r = fifo_server2client.rows-1; r >= 0; r--) {
            if (game[r*fifo_server2client.cols+n-1] == ' '){
                game[r*fifo_server2client.cols+n-1] = fifo_server2client.game_piece;
                break;
            }
        }
                
    }else{
        printf("It's your turn! In which column you want to place your game piece: ");
        do{
            scanf("%d", &n);
            if (n <= 0)
                printf("the value must be greater than 0. Try again: ");
            else if (n>fifo_server2client.cols)
                printf("the value must be lower than %d.  Try again: ", fifo_server2client.cols);
            else if (game[0*fifo_server2client.cols+n-1] != ' ')
                printf("this column is full! Try again: ");
        } while (n <= 0 || n>fifo_server2client.cols || game[0*fifo_server2client.cols+n-1] != ' ');


        for (int r = fifo_server2client.rows-1; r >= 0; r--) {
            if (game[r*fifo_server2client.cols+n-1] == ' '){
                game[r*fifo_server2client.cols+n-1] = turn;
                return;
            }
        }
    }
}

void print_game(){
    for (int r = 0; r < fifo_server2client.cols; r++) 
        printf(" —— ");
    printf("\n");

    for (int r = 0; r < fifo_server2client.rows; r++) {
        for (int c = 0; c < fifo_server2client.cols; c++) {
            if (c == 0)
                printf("|");
            printf(" %c |", game[r*fifo_server2client.cols+c]);
        }
        printf("\n");
        for (int r = 0; r < fifo_server2client.cols; r++) 
            printf(" —— ");
        printf("\n");
    }
}