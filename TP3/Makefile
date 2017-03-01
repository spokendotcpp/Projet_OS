
CFLAGS=-Wall -g
HEADERS=$(wildcard *.h)

all : simul

simul: cpu.o systeme.o simul.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -vf *.o
