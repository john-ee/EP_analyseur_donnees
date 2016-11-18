APPLI=analyse
CSRC= main.c process_data.c
CC = gcc -Wall -Wextra -g

COBJ=$(CSRC:.c=.o)

.c.o:
	$(CC) -c $*.c

$(APPLI):	$(COBJ)
	$(CC) -o $(APPLI) $(COBJ) -lm

clean:
	rm *.o
