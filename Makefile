NAME = server
SRC = ${NAME}.c
CC = gcc
FLAGS = -Wall

all:
	${CC} ${FLAGS} ${SRC} -o ${NAME}

clean:
	rm -f ${NAME}