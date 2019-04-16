#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -DUSEGLEW -Wall
LIBS=-lglew32 -lglut32cu -lglu32 -lopengl32
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f tree *.o *.a
endif

#  Compile and link
tree:tree.c
	gcc $(CFLG) -o $@ $^   $(LIBS)

#  Clean
clean:
	$(CLEAN)
