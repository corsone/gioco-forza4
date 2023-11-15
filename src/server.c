#include "../inc/server.h"

int rows, cols; // rows, cols of matrix
char *game; // matrix for the game field
char game_pieces[2]; // game pieces of players
int shmId, semId; // share memory id, semaphore set id
key_t semKey; // semaphore set key
int fifoFd; // fifo file descriptor
struct fifo_server2client fifo_server2client; // message from server to client
struct fifo_client2server fifo_client2server, fifo_client2server2; // message from client1, client2 to server
int counter_ctrlc; // counter for CTRL-C


void check_arg(int argc, char *argv[]){
    if(argc != 5){
        printf("Usage: %s <rows> <columns> <player 1 game_piece> <player 2 game_piece>\n", argv[0]);
        exit(0);
    }

    if (atoi(argv[1]) < 5){
        printf("Usage: the number of rows must be greater than 5\n");
        exit(0);
    }
    if (atoi(argv[2]) < 5){
        printf("Usage: the number of rows must be greater than 5\n");
        exit(0);
    }

    game_pieces[0] = argv[3][0];
    game_pieces[1] = argv[4][0];
    rows = atoi(argv[1]);
    cols = atoi(argv[2]);
}

void init_signals(){
    sigset_t sigSet;
    sigfillset(&sigSet);
    sigdelset(&sigSet, SIGINT);
    sigdelset(&sigSet, SIGALRM);
    sigdelset(&sigSet, SIGQUIT);
    if (sigprocmask(SIG_SETMASK, &sigSet, NULL) == -1)
        errExit("sigprocmask failed");
    if (signal(SIGINT, sigHandler) == SIG_ERR || signal(SIGALRM, sigHandler) == SIG_ERR || signal(SIGQUIT, sigHandler) == SIG_ERR)
        errExit("signal failed");
}

void sigHandler(int sig){ 
    switch (sig) {
        case SIGINT: // ctrl c
            if(counter_ctrlc == 0){
                alarm(5);
                printf("<Server> press CTRL-C again to end the game\n");
                counter_ctrlc++;
            }else if (counter_ctrlc != 0){
                alarm(0);

                kill(fifo_client2server.idClient, SIGQUIT);
                kill(fifo_client2server2.idClient, SIGQUIT);
                end_game();
            }
            
            break;
        case SIGALRM: // reset counter ctrl c
            if (counter_ctrlc != 0)
                counter_ctrlc = 0;
            
            break;

        case SIGQUIT: // se un client quitta
            printf("<Server> the game is over because a player has left\n");
            kill(fifo_client2server.idClient, SIGCONT);
            kill(fifo_client2server2.idClient, SIGCONT);
            end_game();

            break;
        default:
            break;
    }
}

void init_semaphore(){
    semKey = ftok("F4server.c", 0);
    semId = create_sem(semKey, 4, IPC_CREAT | IPC_EXCL | S_IWUSR | S_IRUSR);

    unsigned short semInit[] = {0, 1, 0, 1};
    union semun arg;
    arg.array = semInit;
    set_sem(semId, arg);
}

void init_shm(){
    key_t shmKey = ftok("F4server.c", 1);
    shmId = create_shm(shmKey, sizeof(char) * rows * cols, IPC_CREAT | IPC_EXCL | S_IWUSR | S_IRUSR);

    game = (char *) attach_shm(shmId);

    semOp(semId, 3, -1); // blocco accesso memoria condivisa
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            game[i*cols+j] = ' ';
        }
    }
    semOp(semId, 3, 1); // sblocco accesso memoria condivisa
}

void write_fifoS2C(char game_piece, char game_piece_opp, int n_player, bool bot){
    fifoFd = open_fifo("fifoS2C", O_WRONLY);
    fifo_server2client.idServer = getpid();
    fifo_server2client.game_piece = game_piece;
    fifo_server2client.game_piece_opp = game_piece_opp;
    fifo_server2client.rows = rows;
    fifo_server2client.cols = cols;
    fifo_server2client.n_player = n_player;
    fifo_server2client.bot = bot;
    write_fifo(fifoFd, &fifo_server2client, sizeof(struct fifo_server2client));
    close(fifoFd);
}

void read_fifoC2S(struct fifo_client2server *fifo_client2server){
    fifoFd = open_fifo("fifoC2S", O_RDONLY);
    read_fifo(fifoFd, fifo_client2server, sizeof(struct fifo_client2server));
    close(fifoFd);
}

bool win(){
    for (int r = 0; r < rows-3; r++) {
        for (int c = 0; c < cols; c++) {
            if (game[r*cols+c] != ' ' && game[r*cols+c] == game[(r+1)*cols+c] && game[(r+1)*cols+c] == game[(r+2)*cols+c] && game[(r+2)*cols+c] == game[(r+3)*cols+c])
                return true;
        }
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols-3; c++) {
            if (game[r*cols+c] != ' ' && game[r*cols+c] == game[r*cols+c+1] && game[r*cols+c+1] == game[r*cols+c+2] && game[r*cols+c+2] == game[r*cols+c+3])
                return true;
        }
    }

    for (int r = 0; r < rows-3; r++) {
        for (int c = 0; c < cols-3; c++) {
            if (game[r*cols+c] != ' ' && game[r*cols+c] == game[(r+1)*cols+c+1] && game[(r+1)*cols+c+1] == game[(r+2)*cols+c+2] && game[(r+2)*cols+c+2] == game[(r+3)*cols+c+3])
                return true;
        }
    }

    for (int r = 0; r < rows - 3; r++) {
        for (int c = cols - 1; c >= 3; c--) {
            if (game[r*cols+c] != ' ' && game[r*cols+c] == game[(r+1)*cols+c-1] && game[(r+1)*cols+c-1] == game[(r+2)*cols+c-2] && game[(r+2)*cols+c-2] == game[(r+3)*cols+c-3])
                return true;
        }
    }

    return false;
}

bool draw(){
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (game[r*cols+c] == ' ')
                return false;
        }
    }

    return true;
}

void play(char turn, char o, char *player){
    if (turn == o){
        printf("<Server> waiting for the first player's move...\n");    
        semOp(semId, 1, 1);
        semOp(semId, 0, -1);
        if (win() == 1){
            kill(fifo_client2server.idClient, SIGUSR1);
            kill(fifo_client2server2.idClient, SIGUSR2);
            printf("<Server> player %s won!\n", player);
            end_game();

        }else if (draw() == 1){
            kill(fifo_client2server.idClient, SIGTERM);
            kill(fifo_client2server2.idClient, SIGTERM);
            printf("<Server> the match ended in a draw\n");
            end_game();
        }
    }else{
        printf("<Server> waiting for the second player's move...\n");
        semOp(semId, 2, 1);
        semOp(semId, 0, -1);
        if (win()){
            kill(fifo_client2server2.idClient, SIGUSR1);
            kill(fifo_client2server.idClient, SIGUSR2);

            printf("<Server> player %s won!\n", player);
            end_game();
        }else if (draw()){
            kill(fifo_client2server.idClient, SIGTERM);
            kill(fifo_client2server2.idClient, SIGTERM);
            printf("<Server> the match ended in a draw\n");
            end_game();
        } 
    }
}

void end_game(){
    if(shmId != 0){
        detach_shm(game);
        remove_shm(shmId);
    }
    if (semId != 0)
        remove_sem(semId);
    
    remove_fifo("fifoS2C");
    remove_fifo("fifoC2S");

    exit(0);
}