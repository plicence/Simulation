

simu: main.o
		gcc -o simu main.o -lm


main.o: main.c 
		gcc -o main.o -c main.c -lm
		
clean: 
		rm -rf main.o
		rm -rf simu