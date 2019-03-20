FROM gcc:7

RUN apt update && apt install -y libgcrypt-dev check valgrind build-essential
