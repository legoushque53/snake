hello:
	clang -O3 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -Iinclude lib/libraylib.a main.c -o snake
