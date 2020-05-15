

simu: main.o
		gcc -o simu main.o -lm -lgmp


main.o: main.c 
		gcc -o main.o -c main.c -lm -lgmp
		
clean: 
		rm -rf main.o
		rm -rf simu
