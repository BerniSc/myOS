FROM debian:bookworm

RUN apt-get update 
RUN apt-get upgrade -y
RUN apt-get install -y apt-utils
RUN apt-get install -y build-essential

RUN apt-get install -y g++-x86-64-linux-gnu
RUN apt-get install -y gcc-10-x86-64-linux-gnu/

RUN apt-get install -y nasm
RUN apt-get install -y xorriso
RUN apt-get install -y grub-pc-bin
RUN apt-get install -y grub-common

VOLUME /root/env
WORKDIR /root/env
