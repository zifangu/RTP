CFLAGS=-Wall
HEADERS = events.h rtp.h
OBJECTS = main.o events.o rtp.o


emulate: $(OBJECTS)
	gcc $(OBJECTS) -o emulate

main.o: main.c $(HEADERS)
	gcc $(CFLAGS) -c main.c

events.o: events.c $(HEADERS)
	gcc $(CFLAGS) -c events.c

rtp.o: rtp.c $(HEADERS)
	gcc $(CFLAGS) -c rtp.c

clean:
	-rm -f $(OBJECTS)
	-rm -f emulate
