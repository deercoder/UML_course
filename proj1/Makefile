proddonuts consdonuts: donuts.h proddonuts.o consdonuts.o utilities.o
	gcc -g -o proddonuts proddonuts.o utilities.o
	gcc -g -o consdonuts consdonuts.o utilities.o

utilities.o: utilities.c
	gcc -g -c utilities.c

consdonuts.o: donuts.h consdonuts.c
	gcc -g -c consdonuts.c

proddonuts.o: donuts.h proddonuts.c
	gcc -g -c proddonuts.c

clean:
	rm *.o proddonuts consdonuts
