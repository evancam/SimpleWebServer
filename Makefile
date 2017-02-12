
all: clean sws

run: clean sws
	./sws 8080 www

sws:
	gcc sws.c -o sws -Wall

clean:
	rm -rf *.out sws	
