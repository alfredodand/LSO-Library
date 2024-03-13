-- init.sql

DO $$
BEGIN
    IF NOT EXISTS (SELECT FROM pg_database WHERE datname = 'libreria') THEN
        CREATE DATABASE libreria;
    END IF;
END $$;

\connect libreria

-- \c libreria;
ALTER USER postgres WITH PASSWORD '0000';

CREATE TYPE genere AS ENUM ('classico', 'fantasy', 'storico'); CREATE TABLE utente(id_utente SERIAL PRIMARY KEY, username VARCHAR(50) NOT NULL, password VARCHAR(50) NOT NULL); CREATE TABLE libro (id_libro SERIAL PRIMARY KEY, titolo VARCHAR(50) NOT NULL, autore VARCHAR(50) NOT NULL, genere GENERE NOT NULL, copie_disponibili INTEGER NOT NULL, copie_in_prestito INTEGER NOT NULL); CREATE TABLE prestiti (id_prestito SERIAL PRIMARY KEY, id_libro INTEGER, nome_utente VARCHAR(50), data_prestito DATE, data_restituzione DATE, FOREIGN KEY (id_libro) REFERENCES libro(id_libro)); CREATE TABLE carrello (nome_utente VARCHAR(50), id_libro INTEGER); CREATE VIEW magazzino AS SELECT titolo, autore, copie_disponibili FROM libro GROUP BY titolo, autore, copie_disponibili;

INSERT INTO utente (username, password) VALUES ('Nome0', '0000'), ('Nome1', '1111'), ('Nome2', '2222'); INSERT INTO libro (titolo, autore, genere, copie_disponibili, copie_in_prestito) VALUES ('Libro1', 'Autore1', 'classico', 5, 0), ('Libro2', 'Autore1', 'classico', 2, 0), ('Libro3', 'Autore2', 'classico', 5, 0), ('Libro4', 'Autore2', 'classico', 3, 0), ('Libro5', 'Autore3', 'classico', 5, 0), ('Libro6', 'Autore4', 'classico', 4, 0), ('Libro7', 'Autore5', 'classico', 5, 0), ('Libro8', 'Autore5', 'classico', 5, 0), ('Libro9', 'Autore5', 'classico', 4, 0), ('Libro10', 'Autore6', 'classico', 6, 0), ('Libro11', 'Autore6', 'classico', 5, 0), ('Libro12', 'Autore6', 'classico', 1, 0), ('Libro13', 'Autore7', 'fantasy', 1, 0), ('Libro14', 'Autore8', 'fantasy', 7, 0), ('Libro15', 'Autore9', 'fantasy', 2, 0), ('Libro16', 'Autore9', 'fantasy', 2, 0), ('Libro17', 'Autore10', 'storico', 3, 0), ('Libro18', 'Autore10', 'storico', 5, 0), ('Libro19', 'Autore11', 'storico', 3, 0), ('Libro20', 'Autore11', 'storico', 5, 0);


