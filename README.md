- [Rules](#rules)
- [Functionality](#functionality)
  - [Server](#server)
  - [Client](#client)
- [Additional Features](#additional-features)
  - [Time-out for Each Move](#time-out-for-each-move)
  - [An Automatic Playing Client](#an-automatic-playing-client)
- [Interactions Between Client and Server](#interactions-between-client-and-server)
- [Design Choices](#design-choices)
  - [Signals](#signals)
  - [Semaphores](#semaphores)
  - [Shared Memory](#shared-memory)
  - [FIFOs](#fifos)
- [Game Management](#game-management)



The game is an application written in C that utilizes SYSTEMV system calls. It is designed to run in a UNIX/LINUX environment.

To compile the game, navigate to the main directory (the one containing the makefile) and run the following command in the terminal:
```
make
```

After compiling, you can delete all object files (.o) contained in the src directory by running:
```
make clean
```

Next, from the current directory, navigate to the directory containing the executables (obj) with:
```
cd obj
```

Finally, open two additional terminals in the obj directory and start the server in one terminal and the clients in the other two([see section](#functionality)):
```
./F4server row col gettone1 gettone2
```

```
./F4client nomeGiocatore
```

## Rules
The game is played between two players on a rectangular board of at least 5x5. Each player takes turns dropping their token, which will settle at the bottom or on top of an existing token in the column.

The player who first aligns four of their own tokens in a row, either vertically, horizontally, or diagonally, wins the game.

## Functionality

The application consists of two main executables:

- F4Server: Initializes the game and referees the match between the two players.
- F4Client: Manages the gameplay for a single player.

### Server

When executed, the server allows you to define the size of the game board (minimum 5x5) and two additional parameters representing the tokens used by the two players in the game.

The execution command will be:
```
./F4Server row col O X
```

Where `row` indicates the number of rows on the game board, `col` indicates the number of columns, and `O` and `X` are the tokens used by the two players (example tokens; any characters can be used).

If the server receives two consecutive `CTRL-C` commands, it will terminate the game and notify the player processes that the game has been terminated externally.

The server is responsible for "refereeing" the game, i.e., determining after each move whether the player has won or not. The server will also notify the clients whether they have won or not. Additionally, it will inform the clients when no more moves can be made and that the game has ended in a draw.

### Client

To start the client, it needs one argument: the player's name. The execution command will be:
```
./F4Client nomeUtente
```

Once launched, the client will wait for the "second player" to be found before the game can start.

When the second player connects, the game can begin.

Pressing CTRL-C on a client is considered as abandoning the game, resulting in the player losing the match.

## Additional Features

### Time-out for Each Move

A time-out of 30 seconds is defined, within which each client must make a move. If a move is not made within this time, the game is declared won by default for the second player.

### An Automatic Playing Client

This is done by randomly generating a number representing the column for play.

To start an automatic client, use the specific command-line option:
```
./F4Client nomeUtente \*
```

## Interactions Between Client and Server

Interactions between the server and client occur through shared memory, two FIFOs, and signals.

The shared memory contains the game board: it is used by the server to referee the game, while the client accesses it to display the game board and make moves. For more details, see the [dedicated section](#shared-memory).

The FIFO server-to-client is written by the server and read by the client. It sends useful information from the server to the client. The second FIFO is from the client to the server. For more details, see the [dedicated section](#fifos).

Finally, signals are used to notify processes of certain actions, such as a player abandoning the game, the server shutting down, or a player winning. For more details on signals, see the [dedicated section](#signals).

## Design Choices

### Signals

Signals used by the server:

- SIGINT: Used to terminate the program by pressing CTRL + C twice. This functionality is managed through a counter and the alarm() system call.
- SIGALRM: Used to reset the counter used by SIGINT.
- SIGQUIT: Used to terminate the game when a player abandons or fails to make a move within the time limit. It also notifies the client of the event.

Signals used by the client:

- SIGINT: Used to indicate that the server has been closed and then terminates the match.
- SIGQUIT: Used to indicate that a client is abandoning the game. It sends the SIGQUIT signal to the server, which responds with SIGCONT and then terminates.
- SIGCONT: Used to indicate that the opponent has abandoned the game. The client that did not abandon the game is declared the winner, and the game ends.
- SIGUSR1: Used to indicate that the client has won.
- SIGUSR2: Used to indicate that the client has lost.
- SIGTERM: Used to indicate that the game has ended in a draw.
- SIGALRM: Used to notify that the time for making a move has expired, resulting in the player losing and notifying the opponent that the game has ended.

### Semaphores

The semaphore set is created by the server using the `ftok()` system call and contains four semaphores:

0. Used to block/unblock the server.
1. Used to block/unblock client 1.
2. Used to block/unblock client 2.
3. Used for access to shared memory.

### Shared Memory

Shared memory is created by the server using the `ftok()` system call and contains only the game board, which is a dynamic matrix. Mutual exclusion for writing (when initializing the game board or when a player makes a move) is managed using a semaphore.

### FIFOs

Communication of useful game information occurs through two FIFOs: one created by the server and directed towards the client, and another created by the client and directed towards the server. The first FIFO is responsible for sending to the client:
- The server's PID (used to send various signals).
- The tokens assigned to the player and their opponent.
- The number of columns and rows on the game board.
- The player's number (first or second).
- Whether the player is a bot playing automatically or a real player.

The second FIFO is responsible for sending to the server:
- The client's PID (used to send various signals).
- The player's name.
- Whether the player wants to play against an opponent or the bot.

## Game Management

The game is refereed by the server, which manages turns between the two players. Specifically, through semaphores, it unlocks the clients and, after each move, checks if the game is over due to a win or a draw. These outcomes are notified to the clients via SIGUSR and SIGTERM signals, and the game then ends.

If the player has chosen to play alone, the server creates a child process using the `fork()` system call and uses `execv()` to execute the client code.

The client handles the 30-second time limit for making a move, facilitates player turns, and displays the game board. If the player is a bot, the move is generated randomly; otherwise, the player is prompted to enter the column number where they want to place their token.