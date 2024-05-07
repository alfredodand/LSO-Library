# Usa un'immagine di base di Ubuntu
FROM ubuntu:latest

# Installa le dipendenze necessarie per il server C e il client PostgreSQL
# RUN apt-get update && \
#     apt-get install -y \
#     build-essential \
#     gcc \
#     make \
#     libc6-dev \
#     postgresql-client \
#     libpq-dev \
#     postgresql-all \
#     apt install ranger -y \
#     mkdir cfiles \
#     cd cfiles/
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Rome
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y \
    gcc \
    make \
    libpq-dev \
    postgresql-all -y
    # echo "tzdata tzdata/Areas select 8" | debconf-set-selections && echo "tzdata tzdata/Zones/Europe select 41" | debconf-set-selections && apt-get install postgresql-all -y


# Imposta la directory di lavoro nel container
WORKDIR /app

# Copia il codice sorgente del server nel container
COPY server.c /app
COPY start.sh /app/start.sh

# Compila il codice sorgente
RUN gcc -o server server.c -I/usr/include/postgresql -lpq

# Esponi la porta del server
EXPOSE 8080

# Avvia il server quando il container viene avviato
CMD ["./bin/bash"]
# CMD ["/app/server"]