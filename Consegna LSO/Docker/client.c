#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 4096
#define MAX_BUFFER 1024

char username[MAX_BUFFER];
char password[MAX_BUFFER];
char title[MAX_BUFFER];
char author[MAX_BUFFER];
int quantity;
char gender[MAX_BUFFER];
int numBooks;
int max_libri_prestati = 10;

void sendRequest(int clientSocket, const char *request) {
    send(clientSocket, request, strlen(request), 0);
    printf("Inviata richiesta al server: %s\n", request);
}

void sendLoginRegisterRequest(int clientSocket, char *username, char *password, const char *requestType) {
    char request[MAX_BUFFER_SIZE];
    snprintf(request, sizeof(request), "%s %s %s", requestType, username, password);
    send(clientSocket, request, strlen(request), 0);
    printf("Inviata richiesta di %s al server: %s\n", requestType, request);
}

void sendAddBookRequest(int clientSocket, char *title, char *author, int quantity, char *gender, const char *requestType) {
    char request[MAX_BUFFER_SIZE];
    snprintf(request, sizeof(request), "%s %s %s %d %s", requestType, title, author, quantity, gender);
    send(clientSocket, request, strlen(request), 0);
    printf("Inviata richiesta di %s al server: %s\n", requestType, request);
}

void receiveResponse(int clientSocket, char *buffer) {
    recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
    printf("Ricevuta risposta dal server: \n%s\n\n", buffer);
}

void printDateTime() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date[26];
    strftime(date, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("\033[s\033[1;37m\033[48;5;235m%s\033[0m\033[u", date);
    fflush(stdout);
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[MAX_BUFFER_SIZE];

    // Creazione del socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo del server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        perror("Errore nella conversione dell'indirizzo IP");
        exit(EXIT_FAILURE);
    }

    // Connessione al server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Errore nella connessione al server");
        exit(EXIT_FAILURE);
    }

    int choice;
    int loginSuccess = 0;
    bool exitMenu = 0;
    // do {
        //printf("\033[H\033[J"); // Pulisce lo schermo
        printf("\033[1;36mBenvenuto!\033[0m\n");
        printf("1. Login\n");
        printf("2. Registrati\n");
        printf("3. Esci\n");
        printDateTime();
        printf("\n\nScelta: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                do {
                    // Ripulire le variabili per evitare valori residui
                    memset(username, 0, sizeof(username));
                    memset(password, 0, sizeof(password));
                    memset(buffer, 0, sizeof(buffer));

                    printf("\nInserisci il tuo username: ");
                    scanf("%s", username);
                    printf("Inserisci la tua password: ");
                    scanf("%s", password);

                    sendLoginRegisterRequest(clientSocket, username, password, "LOGIN");
                    receiveResponse(clientSocket, buffer);

                    if (strncmp(buffer, "LOGIN_OK", 8) == 0) {
                        printf("Login effettuato con successo\n");
                        loginSuccess = 1;
                        int copie_prestate;

                        sendRequest(clientSocket, "COPIE_PRESTATE");
                        receiveResponse(clientSocket, buffer);
                        copie_prestate = atoi(buffer);

                        sendRequest(clientSocket, "NUM_BOOKS");
                        receiveResponse(clientSocket, buffer);
                        numBooks = atoi(buffer);

                        int choice_2 = 0;
                        int choice_3 = 0;
                        do {
                            //printf("\033[H\033[J"); // Pulisce lo schermo
                            printf("\033[1;36mLibreria\033[0m\n");
                            printf("1. Visualizza inventario\n");
                            printf("2. Aggiungi al carrello\n");
                            printf("3. Rimuovi libro dal carrello\n");
                            printf("4. Visualizza carrello\n");
                            printf("5. Checkout\n");
                            printf("6. Esci\n");
                            printf("Scelta: ");
                            scanf("%d", &choice_2);

                            switch(choice_2) {
                                case 1:
                                    do {
                                        printf("\nSeleziona filtro: ");
                                        int filter = 0;
                                        char filter_type[50];
                                        char question[100];
                                        printf("\n0. Cerca seza filtro");
                                        printf("\n1. Cerca per titolo");
                                        printf("\n2. Cerca per genere");
                                        printf("\n3. Cerca per autore");
                                        printf("\n4. Torna al menu principale\n");
                                        printf("\nScelta: ");
                                        scanf("%d", &filter);
                                        memset(buffer, 0, sizeof(buffer));

                                        switch(filter) {
                                            case 0:
                                                sendRequest(clientSocket, "INVENTORY NONE");
                                                receiveResponse(clientSocket, buffer);
                                                break;
                                            case 1:
                                                printf("\nInserisci il titolo: ");
                                                scanf("%s", filter_type);
                                                sprintf(question, "INVENTORY TITLE '%s'", filter_type);

                                                sendRequest(clientSocket, question);
                                                receiveResponse(clientSocket, buffer);
                                                break;
                                            case 2:
                                                printf("\nInserisci il genere: ");
                                                scanf("%s", filter_type);
                                                sprintf(question, "INVENTORY GENDER '%s'", filter_type);

                                                sendRequest(clientSocket, question);
                                                receiveResponse(clientSocket, buffer);
                                                break;
                                            case 3:
                                                printf("\nInserisci l'autore: ");
                                                scanf("%s", filter_type);
                                                sprintf(question, "INVENTORY AUTHOR '%s'", filter_type);

                                                sendRequest(clientSocket, question);
                                                receiveResponse(clientSocket, buffer);
                                                break;
                                            case 4:
                                                exitMenu = 1;
                                                break;
                                        }
                                    } while (!exitMenu);
                                    break;
                                case 2:
                                    char prestito[200];
                                    char question[1000];
                                    if(copie_prestate >= max_libri_prestati){
                                        printf("\nHai già troppi libri in prestito.");
                                    } else {
                                        printf("\nHai a disposizione %d libri da poter prendere in prestito", (max_libri_prestati - copie_prestate));
                                        do {
                                            printf("Inserisci il nome del libro che vuoi prendere in prestito (0 se hai finito): ");
                                            scanf("\n%s", prestito);
                                            sprintf(question, "P_CARRELLO %s", prestito);

                                            sendRequest(clientSocket, question);
                                            receiveResponse(clientSocket, buffer);

                                            memset(buffer, 0, sizeof(buffer));
                                            memset(question, 0, sizeof(question));
                                            memset(prestito, 0, sizeof(prestito));
                                        } while(strncmp(prestito, "0", 1) == 0);
                                    }
                                    break;
                                case 3:
                                    char elimina[200];
                                    do {
                                        printf("Inserisci il nome del libro che vuoi eliminare dal carrello (0 se hai finito): ");
                                        scanf("\n%s", elimina);
                                        sprintf(question, "M_CARRELLO %s", elimina);

                                        sendRequest(clientSocket, question);
                                        receiveResponse(clientSocket, buffer);

                                        memset(buffer, 0, sizeof(buffer));
                                        memset(question, 0, sizeof(question));
                                        memset(elimina, 0, sizeof(elimina));
                                    } while(strncmp(elimina, "0", 1) == 0);
                                    break;
                                case 4:
                                    sendRequest(clientSocket, "VEDI_CARRELLO");
                                    receiveResponse(clientSocket, buffer);
                                    break;
                                case 5:
                                    sendRequest(clientSocket, "CHECKOUT");
                                    receiveResponse(clientSocket, buffer);

                                    memset(buffer, 0, sizeof(buffer));
                                    break;
                                case 6:
                                    break;
                            }
                        } while(1);
                    } else if(strncmp(buffer, "LOGIN_OP_OK", 11) == 0) {
                        printf("Login effettuato con successo\n");
                        loginSuccess = 1;
                        int copie_prestate;

                        sendRequest(clientSocket, "COPIE_PRESTATE");
                        receiveResponse(clientSocket, buffer);
                        copie_prestate = atoi(buffer);

                        sendRequest(clientSocket, "NUM_BOOKS");
                        receiveResponse(clientSocket, buffer);
                        numBooks = atoi(buffer);

                        int choice_2 = 0;
                        int choice_3 = 0;
                        do {
                            //printf("\033[H\033[J"); // Pulisce lo schermo
                            printf("\033[1;36mGestione del magazzino della libreria\033[0m\n");
                            printf("1. Aggiungi un libro\n");
                            printf("2. Visualizza inventario\n");
                            printf("3. Modifica il numero di libri massimi da prestare\n");
                            printf("4. Visualizza prestiti\n");
                            printf("5. Esci\n");
                            printf("Scelta: ");
                            scanf("%d", &choice_2);

                            switch(choice_2) {
                                case 1:
                                    printf("Inserisci il titolo del libro: ");
                                    scanf(" %[^\n]s", title);
                                    printf("Inserisci l'autore del libro: ");
                                    scanf(" %[^\n]s", author);
                                    printf("Inserisci la quantità disponibile: ");
                                    scanf("%d", &quantity);
                                    printf("Inserisci il numero in corrispondenza al genere tra quelli elencati:\n1. Classico\n2. Fantasy\n3. Storico\n");
                                    scanf("%d", &choice_3);
                                    switch(choice_3) {
                                        case 1:
                                            strcpy(gender, "classico");
                                            break;
                                        case 2:
                                            strcpy(gender, "fantasy");
                                            break;
                                        case 3:
                                            strcpy(gender, "storico");
                                            break;
                                        default:
                                                printf("\033[1;31mScelta non valida. Riprova.\033[0m\n");
                                    }
                                    sendAddBookRequest(clientSocket, title, author, quantity, gender, "ADD_BOOK");
                                    receiveResponse(clientSocket, buffer);
                                    numBooks++;
                                    break;
                                case 2:
                                    do {
                                        printf("\nSeleziona filtro: ");
                                        int filter = 0;
                                        char filter_type[50];
                                        char question[100];
                                        printf("\n0. Cerca seza filtro");
                                        printf("\n1. Cerca per titolo");
                                        printf("\n2. Cerca per genere");
                                        printf("\n3. Cerca per autore");
                                        printf("\n4. Torna al menu principale\n");
                                        printf("\nScelta: ");
                                        scanf("%d", &filter);
                                        memset(buffer, 0, sizeof(buffer));

                                        switch(filter) {
                                            case 0:
                                                sendRequest(clientSocket, "INVENTORY NONE");
                                                receiveResponse(clientSocket, buffer);
                                                break;
                                            case 1:
                                                printf("\nInserisci il titolo: ");
                                                scanf("%s", filter_type);
                                                sprintf(question, "INVENTORY TITLE '%s'", filter_type);

                                                sendRequest(clientSocket, question);
                                                receiveResponse(clientSocket, buffer);
                                                break;
                                            case 2:
                                                printf("\nInserisci il genere: ");
                                                scanf("%s", filter_type);
                                                sprintf(question, "INVENTORY GENDER '%s'", filter_type);

                                                sendRequest(clientSocket, question);
                                                receiveResponse(clientSocket, buffer);
                                                break;
                                            case 3:
                                                printf("\nInserisci l'autore: ");
                                                scanf("%s", filter_type);
                                                sprintf(question, "INVENTORY AUTHOR '%s'", filter_type);

                                                sendRequest(clientSocket, question);
                                                receiveResponse(clientSocket, buffer);
                                                break;
                                            case 4:
                                                exitMenu = 1;
                                                break;
                                        }
                                    } while (!exitMenu);
                                    break;
                                case 3:
                                    int prestiti;
                                    char question[1024];
                                    printf("Inserisci il numero massimo di libri da poter prendere in prestito (ora sono %d): ", max_libri_prestati);
                                    scanf("\n%d", &prestiti);
                                    max_libri_prestati = prestiti;
                                    sprintf(question, "MAX_PRESTITI %d", prestiti);

                                    sendRequest(clientSocket, question);
                                    receiveResponse(clientSocket, buffer);
                                    break;
                                case 4:
                                    sendRequest(clientSocket, "VEDI_PRESTITI");
                                    receiveResponse(clientSocket, buffer);
                                    break;
                                case 5:
                                    break;
                            }
                        } while(1);
                    } else if (strncmp(buffer, "LOGIN_FAIL", 10) == 0) {
                        printf("\nLogin non effettuato correttamente. Riprova.\n");
                    } else {
                        printf("Errore durante il login.\n");
                        break;
                    }
                } while (!loginSuccess);
                break;
            }
            case 2:
                    printf("\nInserisci il tuo username: ");
                    scanf("%s", username);
                    printf("Inserisci la tua password: ");
                    scanf("%s", password);

                    sendLoginRegisterRequest(clientSocket, username, password, "REGISTER");
                    receiveResponse(clientSocket, buffer);
                break;
            case 3:
                printf("\033[1;36mArrivederci!\033[0m\n");
                exit(0);
            default:
                printf("\033[1;31mScelta non valida. Riprova.\033[0m\n");
        }
    //     sleep(1); // Aggiorna la data ogni secondo
    // } while (!loginSuccess);

    close(clientSocket);

    return 0;
}
