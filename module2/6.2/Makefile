all: setup

setup: create_lib create_exe_file

create_exe_file:
					gcc -c contacts.c
					gcc contacts.o -o contacts -L../6.2 -llinked -Wl,-rpath,/home/anarchist/git_repo/eltex_practice/module2/6.2/
					rm *.o
create_lib:
					gcc -c contacts.c
					gcc -c -fPIC contacts_lib.c
					gcc -shared -o liblinked.so contacts_lib.o
					rm *.o
clear:
					rm contacts || true
					rm liblinked.so