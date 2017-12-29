

all: cafeteria.c
	gcc cafeteria.c -o a.out -lpthread

run:
	./a.out
