#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "/usr/include/postgresql/libpq-fe.h"

#define MAX_USERS 10 
#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20

#define MAX_BOOKS 100
#define TITLE_LENGTH 100
#define AUTHOR_LENGTH 50
#define GENDER_LENGTH 20

#define PORT 8080
#define MAX_BUFFER_SIZE 4096

typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} User;

typedef struct {
    char title[TITLE_LENGTH];
    char author[AUTHOR_LENGTH];
    char gender[GENDER_LENGTH];
    int quantity;
} Book;

// typedef struct {
//     Libro libri[MAX_LIBRI];
//     int quantita[MAX_LIBRI];
//     int numLibri;
// } Carrello;

char username[MAX_USERNAME_LENGTH];
char password[MAX_PASSWORD_LENGTH];
int max_libri_prestati = 10;

Book inventory[MAX_BOOKS];
int numBooks = 0;

User user;
User users[MAX_USERS];
int numUsers = 0;

int valread;

void *handle_client(void *arg);

char* printDate() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char *date = (char*)malloc(26);  // 26 characters for the formatted date
    if (date == NULL) {
        // Handle memory allocation failure
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strftime(date, 26, "%Y-%m-%d", tm_info);
    return date;
}

char* printDatePlus() {
    time_t t = time(NULL);
    t += 259200;
    struct tm *tm_info = localtime(&t);
    char *date = (char*)malloc(26);  // 26 characters for the formatted date
    if (date == NULL) {
        // Handle memory allocation failure
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strftime(date, 26, "%Y-%m-%d", tm_info);
    return date;
}

int queryInsert(PGconn *conn, char query[2048]) {
    int return_n = 0;
    PGresult *result = PQexec(conn, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        fprintf(stderr, "%s\n", PQerrorMessage(conn));
        return return_n;
    }
    PQclear(result);
    return ++return_n;
}

int querySelectInt(PGconn *conn, char query[2048]) {
    // Esegui la query
    PGresult *result = PQexec(conn, query);

    // Controlla se l'esecuzione della query è avvenuta con successo
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore nell'esecuzione della query: %s", PQerrorMessage(conn));
        PQclear(result);
        exit(EXIT_FAILURE);
    }

    // Verifica se ci sono risultati
    if (PQntuples(result) == 0) {
        fprintf(stderr, "Nessun risultato trovato per la query: %s", query);
        PQclear(result);
        return 0;
    }

    // Recupera e ritorna il risultato
    int resultValue = atoi(PQgetvalue(result, 0, 0));
    return resultValue;
}

int queryDelete(PGconn *conn, const char *query) {
    int return_n = 0;
    PGresult *result = PQexec(conn, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Errore nell'esecuzione della query: %s", PQerrorMessage(conn));
        PQclear(result);
        return return_n;
    }
    PQclear(result);
    return ++return_n;
}

int querySelectBooks(PGconn *conn, char query[2048], int newSocket) {
    // Esegui la query
    PGresult *result = PQexec(conn, query);

    // Controlla se l'esecuzione della query è avvenuta con successo
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore nell'esecuzione della query: %s", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }

    // Recupera e stampa i risultati
    int numRows = PQntuples(result);
    char response[4096];
    memset(response, 0, sizeof(response));

    printf("Magazzino della libreria:\n\n");

    for (int i = 0; i < numRows; i++) {
        // printf("Titolo: %s ", PQgetvalue(result, i, 0));
        // printf("Autore: %s ", PQgetvalue(result, i, 1));
        // printf("Genere: %s ", PQgetvalue(result, i, 2));
        // printf("Quantità: %s", PQgetvalue(result, i, 3));

        // sprintf(titolo, "Titolo: %s ", PQgetvalue(result, i, 0));
        // sprintf(autore, "Autore: %s ", PQgetvalue(result, i, 1));
        // sprintf(genere, "Genere: %s ", PQgetvalue(result, i, 2));
        // sprintf(quantità, "Quantità: %s", PQgetvalue(result, i, 3));    

        // Accoda il titolo
        strcat(response, "\nTitolo: ");
        strcat(response, PQgetvalue(result, i, 0));
        strcat(response, " ");

        // Accoda l'autore
        strcat(response, "\nAutore: ");
        strcat(response, PQgetvalue(result, i, 1));
        strcat(response, " ");

        // Accoda il genere
        strcat(response, "\nGenere: ");
        strcat(response, PQgetvalue(result, i, 2));
        strcat(response, " ");

        // Accoda la quantità
        strcat(response, "\nQuantità: ");
        strcat(response, PQgetvalue(result, i, 3));    

        // Aggiungi una nuova riga tra i risultati
        if (i < numBooks) {
            strcat(response, "\n");
        }
    }
    // sprintf(response, "%s %s %s %s", titolo, autore, genere, quantità);

    send(newSocket, response, strlen(response), 0);

    //int resultValue = atoi(PQgetvalue(result, 0, 0));
    return 1;
}

int querySelectCarrello(PGconn *conn, char query[2048], int newSocket) {
    // Esegui la query
    PGresult *result = PQexec(conn, query);

    // Controlla se l'esecuzione della query è avvenuta con successo
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore nell'esecuzione della query: %s", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }

    // Recupera e stampa i risultati
    int numRows = PQntuples(result);
    char response[4096];
    memset(response, 0, sizeof(response));


    for (int i = 0; i < numRows; i++) {
        // Accoda il titolo
        strcat(response, "\nLibro");
        strcat(response, PQgetvalue(result, i, 0));
        strcat(response, " ");

        // int resultValue = atoi(PQgetvalue(result, 0, 0));

        // char query_2[2048];
        // sprintf(query_2, "SELECT titolo FROM libro WHERE id_libro = %d", resultValue);

        // PGresult *result2 = PQexec(conn, query_2);

        // // Controlla se l'esecuzione della query è avvenuta con successo
        // if (PQresultStatus(result2) != PGRES_TUPLES_OK) {
        //     fprintf(stderr, "Errore nell'esecuzione della query: %s", PQerrorMessage(conn));
        //     PQclear(result2);
        //     return 0;
        // }

        // strcat(response, PQgetvalue(result2, i, 0));

        // Aggiungi una nuova riga tra i risultati
        if (i < numBooks) {
            strcat(response, "\n");
        }
    }

    send(newSocket, response, strlen(response), 0);
    return 1;
}

int querySelectPrestiti(PGconn *conn, char query[2048], int newSocket) {
    // Esegui la query
    PGresult *result = PQexec(conn, query);

    // Controlla se l'esecuzione della query è avvenuta con successo
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore nell'esecuzione della query: %s", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }

    // Recupera e stampa i risultati
    int numRows = PQntuples(result);
    char response[4096];
    memset(response, 0, sizeof(response));


    for (int i = 0; i < numRows; i++) {

        strcat(response, "\nID prestito: ");
        strcat(response, PQgetvalue(result, i, 0));
        strcat(response, " ");

        strcat(response, "\nID libro: ");
        strcat(response, PQgetvalue(result, i, 1));
        strcat(response, " ");

        strcat(response, "\nNome Utente: ");
        strcat(response, PQgetvalue(result, i, 2));
        strcat(response, " ");

        strcat(response, "\nData prestito: ");
        strcat(response, PQgetvalue(result, i, 3));
        strcat(response, " ");

        strcat(response, "\nData restituzione: ");
        strcat(response, PQgetvalue(result, i, 4));
        strcat(response, " ");

        if (i < numBooks) {
            strcat(response, "\n");
        }
    }

    send(newSocket, response, strlen(response), 0);
    return 1;
}

// Funzione di controllo credenziali
int login(PGconn *conn, char* username, char* password) {
    // Verifica la lunghezza dei parametri
    if (strlen(username) >= MAX_USERNAME_LENGTH || strlen(password) >= MAX_PASSWORD_LENGTH) {
        fprintf(stderr, "Errore: lunghezza username o password supera il massimo consentito\n");
        return 0;
    }

    // Prepara la query SQL, utilizzando i placeholder per prevenire SQL injection
    char query[2048];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM utente WHERE username = '%s' AND password = '%s'", username, password);
    
    // Esegue la query
    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore durante l'esecuzione della query: %s\n", PQerrorMessage(conn));
        fprintf(stderr, "Query eseguita: %s\n", query);
        PQclear(res);
        return 0;
    }

    int count = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    if(count > 0){
        printf("Utente loggato con successo.\n");
        return 1;
    } else {
        return 0;
    }  
    
}

int registerUser(PGconn *conn, char* username, char* password) {
    if (numUsers >= MAX_USERS) {
        printf("Il database è pieno. Impossibile aggiungere altri utenti.\n");
        return 0;
    }

    char addUserQuery[2048];
    sprintf(addUserQuery, "INSERT INTO utente (username, password) VALUES ('%s', '%s');", username, password);
    if(queryInsert(conn, addUserQuery) == 0) {
        printf("\nErrore durante la registrazione dell'utente");
        return 0;
    } else {
        numUsers++;
        printf("Utente registrato con successo.\n");
        return 1;
    }
}

int addBook(PGconn *conn, char *title, char *author, int quantity, char *gender) {
    if (numBooks >= MAX_BOOKS) {
        printf("Il magazzino è pieno. Impossibile aggiungere altri libri.\n");
        return 0;
    }

    char addBookrQuery[2048];
    sprintf(addBookrQuery, "INSERT INTO libro (titolo, autore, genere, copie_disponibili, copie_in_prestito) VALUES ('%s', '%s', '%s', %d, 0);", title, author, gender, quantity);
    if(queryInsert(conn, addBookrQuery) == 0) {
        printf("\nErrore durante l'inserimento del libro\n");
        return 0;
    } else {
        numBooks++;
        printf("\nLibro aggiunto con successo.\n");
        return 1;
    }
}

int displayInventory(PGconn *conn, char *filter, char *filter_type, int newSocket) {
    if (numBooks == 0) {
        printf("Il magazzino è vuoto.\n");
        return 1;
    } else if(strncmp(filter, "NONE", 4) == 0) {
        char query[2048] = "SELECT titolo, autore, genere, copie_disponibili FROM libro";
        return querySelectBooks(conn, query, newSocket);
    } else if(strncmp(filter, "TITLE", 5) == 0) {
        char query[2048];
        sprintf(query, "SELECT titolo, autore, genere, copie_disponibili FROM libro WHERE titolo = %s;", filter_type);
        return querySelectBooks(conn, query, newSocket);
    } else if(strncmp(filter, "GENDER", 6) == 0) {
        char query[2048];
        sprintf(query, "SELECT titolo, autore, genere, copie_disponibili FROM libro WHERE genere = %s;", filter_type);
        return querySelectBooks(conn, query, newSocket);
    } else if(strncmp(filter, "AUTHOR", 6) == 0) {
        char query[2048];
        sprintf(query, "SELECT titolo, autore, genere, copie_disponibili FROM libro WHERE autore = %s;", filter_type);
        return querySelectBooks(conn, query, newSocket);
    }
    return 0;
}

int displayCarrello(PGconn *conn, int newSocket) {
    if (numBooks == 0) {
        printf("Il carrello è vuoto.\n");
        return 1;
    } else {
        char query[2048];
        sprintf(query, "SELECT id_libro FROM carrello WHERE nome_utente = '%s';", username);
        return querySelectCarrello(conn, query, newSocket);
    }
    return 0;
}

int displayPrestiti(PGconn *conn, int newSocket) {
    if (numBooks == 0) {
        printf("Non ci sono prestiti.\n");
        return 1;
    } else {
        char query[2048];
        sprintf(query, "SELECT * FROM prestiti;");
        return querySelectPrestiti(conn, query, newSocket);
    }
    return 0;
}



int processRequest(PGconn *conn, char *request, char *response, int newSocket) {
    int flag = 0;
    int indice;
    int copie_in_prestito;
    int copie;
    if (strncmp(request, "LOGIN", 5) == 0) {    

        // Estrai username e password dalla richiesta
        sscanf(request, "LOGIN %s %s", username, password);

        if(login(conn, username, password)){

            if(strncmp(username, "Nome0", 5) == 0 && strncmp(password, "0000", 4) == 0){
                strcpy(response, "LOGIN_OP_OK");
            } else {
                strcpy(response, "LOGIN_OK");
            }           
            flag = 1;
        } else {
            strcpy(response, "LOGIN_FAIL");
            // flag = 0;
        }
    } else if(strncmp(request, "REGISTER", 8) == 0){
        char username[50];
        char password[50];

        // Estrai username e password dalla richiesta
        sscanf(request, "REGISTER %s %s", username, password);

        if(registerUser(conn, username, password)){
            strcpy(response, "REGISTER_OK");
            flag = 1;
        } else {
            strcpy(response, "REGISTER_FAIL");
            // flag = 0;
        }
    } else if(strncmp(request, "ADD_BOOK", 8) == 0) {
        char title[50];
        char author[50];
        int quantity;
        char gender[50];

        // Estrai username e password dalla richiesta
        sscanf(request, "ADD_BOOK %s %s %d %s", title, author, &quantity, gender);

        if(addBook(conn, title, author, quantity, gender)){
            strcpy(response, "ADD_BOOK_OK");
            flag = 1;
        } else {
            strcpy(response, "ADD_BOOK_FAIL");
            // flag = 0;
        }
    } else if(strncmp(request, "INVENTORY", 9) == 0) {
        char filter[50];
        char filter_type[50];
        memset(response, 0, sizeof(response));

        // Estrai username e password dalla richiesta
        sscanf(request, "INVENTORY %s %s", filter, filter_type);

        if(displayInventory(conn, filter, filter_type, newSocket)){
            strcpy(response, "\nINVENTORY_OK");
            flag = 0;
        } else {
            strcpy(response, "\nINVENTORY_FAIL");
            // flag = 0;
        }
    } else if(strncmp(request, "NUM_BOOKS", 9) == 0) {
        char query[2048] = "SELECT COUNT(id_libro) AS libri FROM libro";
        numBooks = querySelectInt(conn, query);
        char num_books[40];
        sprintf(num_books, "Il numero di libri è: %d", numBooks);
        strcpy(response, num_books);
    } else if(strncmp(request, "COPIE_PRESTATE", 14) == 0) {
        int copie_prestate;
        char query[2048];
        char str[100];
        sprintf(query, "SELECT COUNT(id_prestito) FROM prestiti WHERE nome_utente = '%s'", username);
        copie_prestate = querySelectInt(conn, query);
        snprintf(str, sizeof(str), "%d", copie_prestate);
        strcpy(response, str);
    } else if(strncmp(request, "CHECKOUT", 8) == 0) {
        int libri_carrello;
        char query_6[2048];
        sprintf(query_6, "SELECT COUNT(id_libro) FROM carrello WHERE nome_utente = '%s'", username);
        libri_carrello = querySelectInt(conn, query_6);

        do {
            if(max_libri_prestati - copie_in_prestito) {
                int id_libro;
                char query_5[2048];
                sprintf(query_5, "SELECT id_libro FROM carrello WHERE nome_utente = '%s'", username);
                id_libro = querySelectInt(conn, query_5);

                char query_3[2048];
                sprintf(query_3, "SELECT copie_disponibili FROM libro WHERE id_libro = '%d'", id_libro);
                copie = querySelectInt(conn, query_3);

                if(copie > 0) {
                    char query_7[2048];
                    sprintf(query_7, "DELETE FROM carrello WHERE id_libro = %d AND nome_utente = '%s'", id_libro, username);
                    queryDelete(conn, query_7);

                    char query[2048];
                    sprintf(query, "INSERT INTO prestiti (id_libro, nome_utente, data_prestito, data_restituzione) VALUES (%d, '%s', '%s', '%s')", id_libro, username, printDate(), printDatePlus());
                    
                    if(queryInsert(conn, query)) {
                        copie_in_prestito++;
                        char query_4[2048];
                        sprintf(query_4, "UPDATE libro SET copie_disponibili = %d, copie_in_prestito = %d WHERE id_libro = '%d'", (copie = (copie > 0) ? (copie - 1) : copie), copie_in_prestito+1, id_libro);
                        queryInsert(conn, query_4);
                        strcpy(response, "CHECKOUT_OK");
                    } else {
                        strcpy(response, "CHECKOUT_FAIL");
                    }
                } else {
                    strcpy(response, "Copie insufficienti");
                }
                libri_carrello--;
            } else {
                printf("Numero massimo di libri in prestito raggiunto");
                break;
            }
        } while(libri_carrello > 0);
        
        
    } else if(strncmp(request, "P_CARRELLO", 10) == 0) {
        char add_book[50];
        int id_libro;
        memset(response, 0, sizeof(response));

        sscanf(request, "P_CARRELLO %s", add_book);

        char query_2[2048];
        sprintf(query_2, "SELECT id_libro FROM libro WHERE titolo = '%s'", add_book);
        id_libro = querySelectInt(conn, query_2);

        char query_3[2048];
        sprintf(query_3, "INSERT INTO carrello VALUES ('%s', %d)", username, id_libro);
        if(queryInsert(conn, query_3)) {
            strcpy(response, "P_CARRELLO_OK");
        } else {
            strcpy(response, "P_CARRELLO_FAIL");
        }            
    } else if(strncmp(request, "VEDI_CARRELLO", 13) == 0) {
        memset(response, 0, sizeof(response));

        if(displayCarrello(conn, newSocket)){
            strcpy(response, "\nVEDI_CARRELLO_OK");
            flag = 0;
        } else {
            strcpy(response, "\nVEDI_CARRELLO_FAIL");
            // flag = 0;
        }
    } else if(strncmp(request, "VEDI_PRESTITI", 13) == 0) {
        memset(response, 0, sizeof(response));

        if(displayPrestiti(conn, newSocket)){
            strcpy(response, "\nVEDI_PRESTITI_OK");
            flag = 0;
        } else {
            strcpy(response, "\nVEDI_PRESTITI_FAIL");
            // flag = 0;
        }
    } else if(strncmp(request, "MAX_PRESTITI", 12) == 0) {
        memset(response, 0, sizeof(response));
        char prestiti[10];
        sscanf(request, "MAX_PRESTITI %9s", prestiti);

        max_libri_prestati = atoi(prestiti);
        strcpy(response, "\nMAX_PRESTITI_OK");
        flag = 0;
    } else if(strncmp(request, "M_CARRELLO", 10) == 0) {
        memset(response, 0, sizeof(response));
        char del_book[50];
        int id_libro;
        int id_libro_2;
        sscanf(request, "M_CARRELLO %s", del_book);

        char query_2[2048];
        sprintf(query_2, "SELECT id_libro FROM libro WHERE titolo = '%s'", del_book);
        id_libro = querySelectInt(conn, query_2);

        char query_5[2048];
        sprintf(query_5, "SELECT id_libro FROM carrello WHERE nome_utente = '%s' AND id_libro = %d", username, id_libro);
        id_libro_2 = querySelectInt(conn, query_5);

        if(id_libro == id_libro_2) {
            char query_4[2048];
            sprintf(query_4, "DELETE FROM carrello WHERE id_libro = %d AND nome_utente = '%s'", id_libro, username);
            if(queryDelete(conn, query_4)) {
                strcpy(response, "M_CARRELLO_OK");
            } else {
                strcpy(response, "M_CARRELLO_FAIL");
            } 
        } else {
            strcpy(response, "M_CARRELLO_NON_PRESENTE");
        }

    } else {
        // Gestisci altre tipologie di richieste qui
        strcpy(response, "Richiesta non eseguita correttamente");
    }
    return flag;
}

// Connessione al database
void databaseConnection() {

    // Connessione al database utilizzando le variabili di ambiente
    PGconn *conn = PQconnectdb("host=database port=5432 dbname=libreria user=postgres password=0000");
    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connessione al database fallita: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        //exit(1);
    } else {
        printf("Connesso\n");
    }

    // const char *checkTablesQuery = "SELECT table_name FROM information_schema.tables WHERE table_schema='public';";
    // PGresult *checkTablesResult = PQexec(conn, checkTablesQuery);
    // if (PQresultStatus(checkTablesResult) != PGRES_TUPLES_OK || PQntuples(checkTablesResult) == 0) {
    //     // Le tabelle non esistono, creale
    //     printf("Le tabelle non esistono, creazione in corso...\n");

    //     // Creazione tabelle e vista
    //     const char *createTablesQuery = "CREATE TYPE genere AS ENUM ('classico', 'fantasy', 'storico'); CREATE TABLE utente(id_utente SERIAL PRIMARY KEY, username VARCHAR(50) NOT NULL, password VARCHAR(50) NOT NULL); CREATE TABLE libro (id_libro SERIAL PRIMARY KEY, titolo VARCHAR(50) NOT NULL, autore VARCHAR(50) NOT NULL, genere GENERE NOT NULL, copie_disponibili INTEGER NOT NULL, copie_in_prestito INTEGER NOT NULL); CREATE TABLE prestiti (id_prestito SERIAL PRIMARY KEY, id_libro INTEGER, nome_utente VARCHAR(50), data_prestito DATE, data_restituzione DATE, FOREIGN KEY (id_libro) REFERENCES libro(id_libro)); CREATE TABLE carrello (nome_utente VARCHAR(50), id_libro INTEGER); CREATE VIEW magazzino AS SELECT titolo, autore, copie_disponibili FROM libro GROUP BY titolo, autore, copie_disponibili;";
    //     PGresult *createTablesResult = PQexec(conn, createTablesQuery);

    //     if (PQresultStatus(createTablesResult) != PGRES_COMMAND_OK) {
    //         fprintf(stderr, "Errore durante la creazione delle tabelle: %s", PQerrorMessage(conn));
    //         PQclear(createTablesResult);
    //         PQfinish(conn);
    //         exit(1);
    //     }
    //     PQclear(createTablesResult);

    //     // Popolamento DB
    //     printf("Popolazione in corso...\n");

    //     const char *populateDBQuery = "INSERT INTO utente (username, password) VALUES ('Nome0', '0000'), ('Nome1', '1111'), ('Nome2', '2222'); INSERT INTO libro (titolo, autore, genere, copie_disponibili, copie_in_prestito) VALUES ('Libro1', 'Autore1', 'classico', 5, 0), ('Libro2', 'Autore1', 'classico', 2, 0), ('Libro3', 'Autore2', 'classico', 5, 0), ('Libro4', 'Autore2', 'classico', 3, 0), ('Libro5', 'Autore3', 'classico', 5, 0), ('Libro6', 'Autore4', 'classico', 4, 0), ('Libro7', 'Autore5', 'classico', 5, 0), ('Libro8', 'Autore5', 'classico', 5, 0), ('Libro9', 'Autore5', 'classico', 4, 0), ('Libro10', 'Autore6', 'classico', 6, 0), ('Libro11', 'Autore6', 'classico', 5, 0), ('Libro12', 'Autore6', 'classico', 1, 0), ('Libro13', 'Autore7', 'fantasy', 1, 0), ('Libro14', 'Autore8', 'fantasy', 7, 0), ('Libro15', 'Autore9', 'fantasy', 2, 0), ('Libro16', 'Autore9', 'fantasy', 2, 0), ('Libro17', 'Autore10', 'storico', 3, 0), ('Libro18', 'Autore10', 'storico', 5, 0), ('Libro19', 'Autore11', 'storico', 3, 0), ('Libro20', 'Autore11', 'storico', 5, 0);";
    //     PGresult *populateDBResult = PQexec(conn, populateDBQuery);

    //     if (PQresultStatus(populateDBResult) != PGRES_COMMAND_OK) {
    //         fprintf(stderr, "Errore durante l'inserimento: %s", PQerrorMessage(conn));
    //     }
    //     PQclear(populateDBResult);
    // }

    // printf("Operazioni completate con successo.\n");

    char query[2048] = "SELECT COUNT(id_libro) AS libri FROM libro";
    numBooks = querySelectInt(conn, query);
    char query_2[2048] = "SELECT COUNT(id_utente) as utenti_totali FROM utente";
    numUsers = querySelectInt(conn, query_2);

    ///////////////////INIZIO SOCKET
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    char buffer[MAX_BUFFER_SIZE];

    // Creazione del socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo del server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind del socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Errore nel binding del socket");
        exit(EXIT_FAILURE);
    }

    // In ascolto per le connessioni in ingresso
    if (listen(serverSocket, MAX_USERS) == -1) {
        perror("Errore nell'ascolto delle connessioni");
        exit(EXIT_FAILURE);
    }

    printf("Server in ascolto sulla porta %d...\n", PORT);

    // Gestione di più client con l'uso di threads
    pthread_t threads[MAX_USERS];
    int client_count = 0;

    // while (1) {
    //     // Accetta la connessione da un client
    //     newSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
    //     if (newSocket == -1) {
    //         perror("Errore nell'accettare la connessione");
    //         exit(EXIT_FAILURE);
    //     }
    //     int flag = 0;
    //     while(true){
    //         memset(buffer, 0, sizeof(buffer));
    //         // Ricevi la richiesta dal client
    //         recv(newSocket, buffer, MAX_BUFFER_SIZE, 0);
    //         printf("Ricevuta richiesta dal client: %s\n", buffer);

    //         // Processa la richiesta e genera una risposta
    //         char response[MAX_BUFFER_SIZE];
    //         flag = processRequest(conn, buffer, response, newSocket);

    //         // Invia la risposta al client
    //         send(newSocket, response, strlen(response), 0);
    //     }

    //     // Chiudi la connessione con il client
    //     close(newSocket);
    // }

    while (1) {
        if ((newSocket = accept(serverSocket, (struct sockaddr *)&serverAddr, (socklen_t*)&addrLen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("New connection, socket fd is %d\n", newSocket);

        if (pthread_create(&threads[client_count], NULL, handle_client, (void *)&newSocket) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        client_count++;
    }

    // Chiudi il socket del server
    close(serverSocket);
    ///////////////////TERMINE SOCKET

    PQfinish(conn);
}

void *handle_client(void *arg) {
    
    // Connessione al database utilizzando le variabili di ambiente
    PGconn *conn = PQconnectdb("host=database port=5432 dbname=libreria user=postgres password=0000");
    int new_socket = *((int *)arg);
    char buffer[1024] = {0};

    // Ricevi dati dal client e rispondi
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        // Ricevi la richiesta dal client
        recv(new_socket, buffer, MAX_BUFFER_SIZE, 0);
        printf("Ricevuta richiesta dal client: %s\n", buffer);

        // Processa la richiesta e genera una risposta
        char response[MAX_BUFFER_SIZE];
        processRequest(conn, buffer, response, new_socket);

        // Invia la risposta al client
        send(new_socket, response, strlen(response), 0);
    }

    // close(new_socket);
    // pthread_exit(NULL);
}

int main() {
    databaseConnection();

    return 0;
}
