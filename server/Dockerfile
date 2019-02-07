FROM gcc:7
RUN apt update && \
        apt install -y libgcrypt-dev
COPY .. server
WORKDIR server
RUN make
EXPOSE 6000
WORKDIR .
ENTRYPOINT ["server", "-v", "-l", "server-logfile", "6000"]