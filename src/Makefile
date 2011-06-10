SOURCES = voz

INCLUDES = -I. -I$(srcdir) -I../include -I ../include
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CFLAGS)
CC = gcc
CFLAGS = -I. -g -Wall -Wshadow -Wpointer-arith -Wwrite-strings -Wno-strict-prototypes

all: $(SOURCES)

.SUFFIXES: .c .o

voz: voz.o audio.o sip.o rtp.o g711.o
	$(CC) $(CFLAGS) -lm $(LDFLAGS) -o $@ voz.o audio.o sip.o rtp.o g711.o

.c.o:   ../sip.h
	$(COMPILE) -c -o $@ $<

clean: clean-SOURCES
	rm -f *.o

clean-SOURCES:
	@list='$(SOURCES)'; for p in $$list; do \
	  f=`echo $$p|sed 's/$$//'`; \
	  echo " rm -f $$p $$f"; \
	  rm -f $$p $$f ; \
	done


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
