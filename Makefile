LIBDIRS = -I/usr/X11R6/include/ -L/usr/X11R6/lib 

	COMPILERFLAGS = -O2 -Wall

CC = gcc
CFLAGS = $(COMPILERFLAGS)
LIBS = -lX11 -lXi -lXmu -lglut -lGL -lGLU -lm

renderISDGLR: isdCore.h isdCore.o isdGL.h isdGL.o app.c
	$(CC) $(CFLAGS) -o isdGLR $(LIBDIRS) isdCore.o isdGL.o app.c $(LIBS)

isdGL.o: isdGL.c isdGL.h isdCore.h
	$(CC)  $(CFLAGS) -c isdGL.c

isdCore.o: isdCore.c isdCore.h 
	$(CC) $(CFLAGS) -c isdCore.c
