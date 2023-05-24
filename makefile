CC = g++
CFLAGS = -std=c++11 -pthread

all: st_pipeline

st_pipeline: st_pipeline.o
	$(CC) $(CFLAGS) -o st_pipeline st_pipeline.o

st_pipeline.o: st_pipeline.cpp
	$(CC) $(CFLAGS) -c st_pipeline.cpp

clean:
	rm -f st_pipeline st_pipeline.o
