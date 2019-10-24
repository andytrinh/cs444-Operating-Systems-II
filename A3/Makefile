#
# Andy Trinh
#

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)

PROG = mystat

all: $(PROG)

mystat:
	gcc -o $(PROG) $(CFLAGS) mystat.c

tar: clean
	tar cvfz $(PROG).tar.gz *.[ch] ?akefile

clean cls:
	rm -f $(PROG) *.o *~ \#*
	
ci:
	ci -m"auto-checkin" -l *.[ch] ?akefile
