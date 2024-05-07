 .----------------. .----------------. .----------------. .----------------. .----------------. .----------------. 
| .--------------. | .--------------. | .--------------. | .--------------. | .--------------. | .--------------. |
| |  _______     | | |  _________   | | |      __      | | |  ________    | | | ____    ____ | | |  _________   | |
| | |_   __ \    | | | |_   ___  |  | | |     /  \     | | | |_   ___ `.  | | ||_   \  /   _|| | | |_   ___  |  | |
| |   | |__) |   | | |   | |_  \_|  | | |    / /\ \    | | |   | |   `. \ | | |  |   \/   |  | | |   | |_  \_|  | |
| |   |  __ /    | | |   |  _|  _   | | |   / ____ \   | | |   | |    | | | | |  | |\  /| |  | | |   |  _|  _   | |
| |  _| |  \ \_  | | |  _| |___/ |  | | | _/ /    \ \_ | | |  _| |___.' / | | | _| |_\/_| |_ | | |  _| |___/ |  | |
| | |____| |___| | | | |_________|  | | ||____|  |____|| | | |________.'  | | ||_____||_____|| | | |_________|  | |
| |              | | |              | | |              | | |              | | |              | | |              | |
| '--------------' | '--------------' | '--------------' | '--------------' | '--------------' | '--------------' |
 '----------------' '----------------' '----------------' '----------------' '----------------' '----------------' 


---------------------------------------------- Installazione e Avvio ----------------------------------------------

0. Requisiti di Sistema:
	Sistema operativo: Linux, macOS o Windows (si consiglia un ambiente Linux).
	Compilatore C (preferibilmente GCC).
	[Server PostgreSQL] (solo in caso non si voglia fare uso dei container Docker).


1. [Configurazione] (solo in caso non si voglia fare uso dei container Docker - Docker è richiesto per il progetto):
	Modificare le impostazioni e i parametri di connessione al database PostgreSQL nel file sorgente del server.


2. Installazione (tra parentesi vi sono i comandi da eseguire dentro la directory "/Docker" per le varie operazioni):
	Clonare o scaricare la repository.
	Compilare il codice del client utilizzando il compilatore C sul proprio sistema (gcc -o client client.c).
	Avviare il server eseguendo il comando di avvio dell'ecosistema Docker per il file ".yml" (docker compose up).
	Attendere il completamento dell'avvio del server e successivamente eseguire il client compilato (./client).

                                                                                       
                                                                                       
