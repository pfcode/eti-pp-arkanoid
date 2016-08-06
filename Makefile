CFLAGS=-lm -lSDL2 -lpthread -ldl -lrt -O2
OBJS=main.cpp map.cpp render.cpp
OUTPUT=arkanoid

all:
	$(CXX) $(CFLAGS) -o $(OUTPUT) $(OBJS)

