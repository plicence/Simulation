

simu: main.o
		gcc -o simu main.o


main.o: main.c 
		gcc -o main.o -c main.c 
		
clean: 
		rm -rf main.o
		rm -rf simu