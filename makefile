mygrep : compregex.o afn.o afd.o mygrep.o
	gcc compregex.o afn.o afd.o mygrep.o -o mygrep

afn.o : afn.h afd.h afn.c
	gcc -c afn.c

afd.o : afd.h afn.h afd.c
	gcc -c afd.c

compregex.o : compregex.h compregex.c
	gcc -c compregex.c

mygrep.o : compregex.h mygrep.c afd.h afn.h
	gcc -c mygrep.c
