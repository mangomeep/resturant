NAME = server
SRC = ${NAME}.c
CC = gcc
FLAGS = #-Wall -Wextra

all: build

build:
	${CC} ${FLAGS} ${SRC} -o ${NAME}
	@ echo compiled!

clean:
	rm -fv ${NAME}
