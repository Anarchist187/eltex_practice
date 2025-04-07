all: setup

setup: create_lib create_exe_file

create_exe_file:
					gcc -c contacts.c
					gcc contacts.o -o contacts -L. contacts_lib.a
					rm *.o
create_lib:
					gcc -c contacts.c contacts_lib.c
					gcc -c contacts.h -o contactsh.o
					ar r contacts_lib.a *.o
					rm *.o
clear:
					rm contacts || true
					rm contacts_lib.a