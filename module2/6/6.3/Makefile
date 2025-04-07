all: setup

setup: create_lib create_exe

create_lib: calc.h
			gcc -c -fPIC add.c
			gcc -shared -o lib/libadd.so add.c
			gcc -c -fPIC sub.c
			gcc -shared -o lib/libsub.so sub.c
			gcc -c -fPIC mult.c
			gcc -shared -o lib/libmult.so mult.c
			gcc -c -fPIC divide.c
			gcc -shared -o lib/libdivide.so divide.c
			gcc -c -fPIC menu.c
			gcc -shared -o lib/libmenu.so menu.c
			rm *.o

create_exe:
			gcc -c reallyhardcalc.c
			gcc -rdynamic reallyhardcalc.o -o reallyhardcalc -ldl
			rm *.o

clear:
			rm reallyhardcalc || true
			rm lib/libadd.so || true
			rm lib/libsub.so || true
			rm lib/libdivide.so || true
			rm lib/libmenu.so || true
			rm *.o || true
			rm lib/libmult.so