all:
	gcc -c -Wall -Werror -fpic UIUX.c
	gcc -shared -o libUIUX.so UIUX.o
	gcc -L $(PWD) -Wl,-rpath=$(PWD) -Wall -Werror -o main.o main.c -lUIUX
	
run:
	./main.o

clean:
	rm *.o *.so