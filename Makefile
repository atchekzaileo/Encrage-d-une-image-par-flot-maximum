CC = g++
CFLAGS = -Wall -Wextra -O2 -std=c++11

OBJ = main.o Image.o Pixel.o MaxFlow.o

tp9: $(OBJ)
	$(CC) $(CFLAGS) -o tp9 $(OBJ)

main.o: main.cpp Image.h Pixel.h MaxFlow.h
	$(CC) $(CFLAGS) -c main.cpp

Image.o: Image.cpp Image.h Pixel.h
	$(CC) $(CFLAGS) -c Image.cpp

Pixel.o: Pixel.cpp Pixel.h
	$(CC) $(CFLAGS) -c Pixel.cpp

MaxFlow.o: MaxFlow.cpp MaxFlow.h Image.h Pixel.h
	$(CC) $(CFLAGS) -c MaxFlow.cpp

clean:
	rm -f *.o tp9
