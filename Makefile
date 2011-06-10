SUBDIRS = src
CADEIRA = RIC
PROJECTO = VOIP
ALUNO1 = 57442
ALUNO2 = 57476
CC = gcc
CFLAGS = -g -O0 -Wall


all: build install

build:
	@list='$(SUBDIRS)'; for p in $$list; do \
	  echo "Building $$p"; \
	  $(MAKE) -C $$p; \
	done

install:
	mv src/voz bin/
	ln -s bin/voz voz 	

clean:
	@list='$(SUBDIRS)'; for p in $$list; do \
	  echo "Cleaning $$p"; \
	  $(MAKE) clean -C $$p; \
	done
	rm -f voz	
	rm -f bin/voz

package: clean zip

zip:
	zip -r ../$(CADEIRA)-$(PROJECTO)-$(ALUNO1)-$(ALUNO2).zip *
