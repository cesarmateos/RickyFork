#Makefile de Sac-Server

CC := gcc
MAJOR := 0
MINOR := 1
NAME := Server
VERSION := $(MAJOR).$(MINOR)
BUILD=bin


all: clean	SAC-Server

clean:
	$(RM) *.o
	$(RM) simple
	$(RM) *.so
	$(RM) -r bin/
	mkdir -p bin

SAC-Server: SAC-Server.o
	$(CC) -o SAC-Server SAC-Server.o