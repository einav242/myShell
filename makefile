
main: main.o 
	gcc -o main main.o 

main.o: main.c  
	gcc  -c main.c

server: server.o 
	gcc -o server server.o 

server.o: server.c  
	gcc  -c server.c

.PHONY: clean
	
clean:
	rm -f *.o  main server