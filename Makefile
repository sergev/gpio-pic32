PROG            = gpio
CFLAGS		= -O -Wall -Werror
LIB             =
OBJ             = main.o gpio.o

all:		$(PROG)

gpio:           $(OBJ)
		$(CC) $(LDFLAGS) $(OBJ) $(LIB) -o $@

clean:
		rm -f $(PROG) *.o
