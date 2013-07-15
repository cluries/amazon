CC = g++
FL = -g -Wall
LB = -lcurl -lpthread -lpcre

objects = filemanager.o property.o amazonit.o clloger.o

all:amazon.cc $(objects)
	$(CC) $(FL) amazon.cc $(objects) -o amazon $(LB)

$(objects):%o:%cc
	$(CC) -c $(FL) $< -o $@

clean:
	rm -rf *.o amazon amazon.dSYM	
