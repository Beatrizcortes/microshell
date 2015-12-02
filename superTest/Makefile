all: count

count: main.o libcount.a
	gcc -g -Wall -o count main.o -L . -l count

main.o: main.c readchar_R.h count_M.h
	gcc -g -Wall -c -o main.o main.c

readchar_R.o: readchar_R.c readchar_R.h count_M.h
	gcc -g -Wall -c -o readchar_R.o readchar_R.c

count_M.o: count_M.c count_M.h readchar_R.h
	gcc -g -Wall -c -o count_M.o count_M.c

libcount.a: readchar_R.o count_M.o
	ar -r libcount.a readchar_R.o count_M.o
clean:
	rm -f count
	rm -f *.o
	rm -f libcount.a
