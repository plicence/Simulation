

simu: main.o file.o
		gcc -o simu main.o file.o -lm -lmpfr -lgmp

main.o: main.c file.h
		gcc -o main.o -c main.c -lm -lmpfr -lgmp

file.o: file.c file.h
		gcc -o file.o -c file.c
		
clean: 
		rm -rf main.o
		rm -rf file.o
		rm -rf simu
