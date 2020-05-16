

simu: main.o
		gcc -o simu main.o -lm -lmpfr -lgmp


main.o: main.c 
		gcc -o main.o -c main.c -lm -lmpfr -lgmp
		
clean: 
		rm -rf main.o
		rm -rf simu
