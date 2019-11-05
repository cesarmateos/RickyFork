#Makefile de Sac-Server

CC := gcc
MAJOR := 0
MINOR := 1
NAME := Cliente
VERSION := $(MAJOR).$(MINOR)
BUILD=bin


all: clean	SAC-Cli

clean:
	$(RM) *.o
	$(RM) simple
	$(RM) *.so
	$(RM) -r bin/
	mkdir -p bin

SAC-Cli: SAC-Cli.o
	$(CC) -o SAC-Cli SAC-Cli.o