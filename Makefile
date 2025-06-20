.DEFAULT_GOAL := all




main.o: ./src/main/main.cpp 
	g++ -c -Iinclude ./src/main/main.cpp -o ./target/main.o



main: $ main.o
	g++ -Iinclude ./target/main.o -o ./target/main

all: clean test

run: main
	./target/main

test: main
	./target/main
   

clean: 
	rm -rf ./target/*.o
	rm -rf ./target/*.exe
