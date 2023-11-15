- [gioco-forza4](#gioco-forza4)
  - [1.1. Le regole](#11-le-regole)
  - [1.2. Funzionamento](#12-funzionamento)
    - [1.2.1. Server](#121-server)
    - [1.2.2. Client](#122-client)
    - [1.2.3. Altre funzionalità](#123-altre-funzionalità)
      - [1.2.3.1. Time-out per ogni mossa](#1231-time-out-per-ogni-mossa)
      - [1.2.3.2. Un client che gioca in modo automatico](#1232-un-client-che-gioca-in-modo-automatico)
  - [1.3. Interazioni fra client e server](#13-interazioni-fra-client-e-server)
  - [1.4. Scelte progettuali](#14-scelte-progettuali)
    - [1.4.1. I segnali](#141-i-segnali)
    - [1.4.2. I semafori](#142-i-semafori)
    - [1.4.3. La memoria condivisa](#143-la-memoria-condivisa)
    - [1.4.4. Le fifo](#144-le-fifo)
  - [1.5. Gestione della partita](#15-gestione-della-partita)

# gioco-forza4

Il gioco è un'applicazione scritta in linguaggio C che sfrutta le system call SYSTEMV. Esso è in grado di funzionare in ambiente UNIX/LINUX. 

Per compilare il gioco bisogna spostarsi nella cartella principale(quella che contiene il make file) e da terminale lanciare il comando:
```
make
```

Una volta fatto ciò si può eliminare tutti i file oggetto(.o) contenuti nella cartella src. Per fare ciò si deve  lanciare il comando:
```
make clean
```

Successivamente, dalla cartella corrente, bisogna spostarsi nella cartella che contiene gli eseguibili(obj) con il comando:
```
cd obj
```

In fine bisogna aprire altri due terminali nella cartella obj e fa partire in uno il server mentre negli altri due i client([vai alla sezione](#12-funzionamento)):
```
./F4server row col gettone1 gettone2
```

```
./F4client nomeGiocatore
```


## 1.1. Le regole

Il gioco si svolge tra due giocatori, su un campo rettangolare di dimensione almeno 5x5 dove ogni giocatore a
turno lascia cadere il proprio gettone che andrà a posarsi sul fondo (o su un gettone già presente in quella
colonna).

Il giocatore che avrà vinto sarà il primo giocatore che sarà riuscito ad allineare 4 propri gettoni senza interruzione.
L'allineamento dei 4 gettoni può avvenire in verticale, orizzontale oppure anche in diagonale.

## 1.2. Funzionamento

L’applicazione è composta da due esegubili principali:
- F4Server che si occupa di inizializzare il gioco, e di arbitrare la partita tra i 2 giocatori
- F4Client che si occupa di far giocare il singolo giocatore

### 1.2.1. Server

Il server, quando viene eseguito, prevede la possibilità di definire la dimensione del campo di gioco (con
un minimo di 5x5) e due parametri aggiuntivi, i quali saranno le forme dei due gettoni uno per
ogni giocatore, utilizzati nella partita.

Quindi la riga di esecuzione sarà:
```
./F4Server row col O X
```

Dove row indica il numero delle righe del campo di gioco, col indica le colonne, O e X sono i gettoni che saranno usati dai due giocatori(sono di esempio, possono essere usati tutti i caratteri che si vogliono).


Il server, qualora venga premuto due volte di
seguito il comando CTRL-C, fa terminare il gioco e avvisa i processi dei giocatori che stanno giocando che il gioco è stato terminato dall’esterno.

È compito del server “arbitrare” la partita, ovvero sarà il server a decidere dopo ogni giocata se il giocatore che
ha giocato ha vinto o meno la partita, il server dovrà segnalare di conseguenza ai client anche se hanno vinto o
meno. Inoltre si occupa di notificare ai client, quando non sono più possibili inserimenti di gettoni che la partita è finita alla pari.

### 1.2.2. Client

Il client per essere avviato ha bisogno di un argomento: il nome del giocatore, quindi la riga di
esecuzione sarà:
```
./F4Client nomeUtente
```

Una volta lanciato il client, rimarrà in attesa che venga “trovato il secondo giocatore” dopo di che il gioco potrà
iniziare.

Quando il secondo giocatore si collega la partita può iniziare.

La pressione di CTRL-C su un client viene considerato come un abbandono, di conseguenza il giocatore perde la partita.

### 1.2.3. Altre funzionalità

#### 1.2.3.1. Time-out per ogni mossa

È stato definito un numero di secondi di time-out(30 secondi), entro il quale ogni client deve obbligatoriamente giocare, qualora non giochi entro quel tempo, si dichiara la partita vinta a tavolino dal secondo giocatore.

#### 1.2.3.2. Un client che gioca in modo automatico

Questo avviene banalmente con una generazione causale di un numero che rappresenta la colonna di gioco.

In questo bisogna far partire un solo client con una specifica opzione da riga di comando (*):
```
./F4Client nomeUtente \*
```

## 1.3. Interazioni fra client e server

Le interazioni fra il server e il client avvengono attraverso la memoria condivisa, due fifo e alcuni segnali.

La memoria condivisa contiene il campo di gioco: viene usata dal server per arbitrare la partita, mentre il client accede per stampare il campo di gioco ed effettuare la mossaPer maggiori informazioni visitare la [sezione dedicata](#143-la-memoria-condivisa).

La fifo server to client viene scritta dal server e letta dal client. Il suo scopo è quello di inviare delle informazioni utili dal server al client. La seconda fifo va dal client al server. Per maggiori informazioni visitare la [sezione dedicata](#144-le-fifo).

Infine ci sono i segnali, i quali vengono utilizzati per notificare ai processi che è avvenuta una determinata azione, come ad esempio l’abbandono di uno dei due gioca- tori, la chiusura del server, la vincita di un giocatore, ecc. Per maggiori informazioni riguardo ai segnali visitare la [sezione dedicata](#141-i-segnali).

## 1.4. Scelte progettuali

### 1.4.1. I segnali

Per quanto riguarda i segnali utilizzati dal server:
- SIGINT, il segnale viene utilizzato per terminare il programma premendo due volte CTRL + C. Questa funzionalità avviene attraverso un contatore e l’utilizzo della system call alarm()
- SIGALRM, viene utilizzato per resettare il contatore usato da SIGINT
- SIGQUIT, questo segnale viene usato per terminare la partita quando un gio- catore abbandona il gioco o non effettua una mossa entro lo scadere del tempo. Inoltre, notifica ciò che è avvenuto al client
I segnali utilizzati dal client sono:
- SIGINT, viene usato per indicare che il server è stato chiuso e poi termina la partita
- SIGQUIT, viene utilizzato per indicare che un client abbandona la partita, per notificare ciò al server invia il segnale SIGQUIT al server, il quale risponde con il segnale SIGCONT e poi termina
- SIGCONT, viene usato per indicare che l’avversario ha abbandonato la partita. La vittoria viene assegnata al client che non ha abbandonato e poi la partita termina
- SIGUSR1, viene usato per indicare che il client ha vinto
- SIGUSR2, viene usato per indicare che il client ha perso
- SIGTERM, viene usato per indicare che la partita è terminata in pareggio
- SIGALRM, viene usato per notificare che il tempo per effettuare la mossa è scaduto, facendo perdere il giocatore e notificando all’avversario che il gioco è terminato.

### 1.4.2. I semafori

Il set di semafori viene creato dal server generando la chiave tramite la system call ftok() e contiene quattro semafori:

0. utilizzato per bloccare/sbloccare il server
1. utilizzato per bloccare/sbloccare il client 1
2. utilizzato per bloccare/sbloccare il client 2
3. utilizzato per l’accesso alla memoria condivisa

### 1.4.3. La memoria condivisa

La memoria condivisa viene creata dal server generando la chiave tramite la system call ftok() e contiene solamente il campo di gioco, ovvero una matrice dinamica. La mutua esclusione in scrittura (quando il campo di gioco viene inizializzato oppure quando un giocatore effettua una mossa) avviene tramite l’utilizzo di un semaforo.

### 1.4.4. Le fifo

La comunicazione delle informazioni utili per il gioco avviene attraverso l’utilizzo di due fifo, una creata dal server e diretta verso il client e un’altra creata dal client e diretta verso il server. La prima si occupa di inviare al client:
- il pid del server (utilizzato per inviare i vari segnali)
- la pedine assegnate al giocatore e al suo avversario
- il numero di colonne e righe del campo di gioco
- il numero del giocatore (primo o secondo)
- se il giocatore è un bot che gioca in modo automatico oppure è un giocatore vero e proprio

La seconda si occupa di inviare al server:
- il pid del client (utilizzato per inviare i vari segnali)
- il nome del giocatore
- se il giocatore vuole giocare con un avversario oppure contro il bot

## 1.5. Gestione della partita

La partita viene arbitrata dal server, il quale si occupa di gestire i turni fra i due giocatori. In particolare, attraverso i semafori, si occupa di sbloccare i client e, a seguito di una mossa, verificare se la parita è finita per una vincita oppure per un pareggio, questi due casi verranno notificati al client tramite i segnali SIGUSR e SIGTERM e poi terminerà.

Nel caso in cui il giocatore abbia deciso di giocare da solo il server crea un figlio tramite la system call fork() e tramite la execv() gli fa eseguire il codice del client.

Il client si occupa della gestione del tempo(30 secondi) per fare la mossa, di far giocare i giocatori e di stampare il campo di gioco. Se il giocatore è un bot la mossa viene generata casualmente, altrimenti viene chiesto di inserire il numero della colonna in cui si vuole posizionare la propria pedina.