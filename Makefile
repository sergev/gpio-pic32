PROG            = gpio
DESTDIR         = /usr/local/bin
CFLAGS		= -O -Wall -Werror
LIB             =
OBJ             = main.o gpio.o alt.o

all:		$(PROG)

gpio:           $(OBJ)
		$(CC) $(LDFLAGS) $(OBJ) $(LIB) -o $@

clean:
		rm -f $(PROG) *.o

install:        gpio
		sudo cp -a gpio $(DESTDIR)/gpio
		sudo chown root $(DESTDIR)/gpio
		sudo chmod u+s $(DESTDIR)/gpio
