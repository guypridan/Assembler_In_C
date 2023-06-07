
A_project: A_project.o input_handling.o phase_1.o phase_2.o macro_handling.o label_handling.o export.o error_handling.o a_project.h
	gcc -g -Wall -ansi -pedantic  A_project.o input_handling.o phase_1.o phase_2.o macro_handling.o label_handling.o export.o error_handling.o a_project.h -o A_project

A_project.o: A_project.c
	gcc -g -c  -Wall -ansi -pedantic A_project.c -o A_project.o

phase_1.o:  phase_1.c
	gcc -g -c -Wall -ansi -pedantic phase_1.c -o phase_1.o 

phase_2.o:  phase_2.c
	gcc -g -c -Wall -ansi -pedantic phase_2.c -o phase_2.o 

macro_handling.o:  macro_handling.c
	gcc -g -c -Wall -ansi -pedantic macro_handling.c -o macro_handling.o 

input_handling.o:  input_handling.c
	gcc -g -c -Wall -ansi -pedantic input_handling.c -o input_handling.o 

label_handling.o:  label_handling.c
	gcc -g -c -Wall -ansi -pedantic label_handling.c -o label_handling.o 

export.o:  export.c
	gcc -c -g -Wall -ansi -pedantic export.c -o export.o

error_handling.o:  error_handling.c
	gcc -c -g -Wall -ansi -pedantic error_handling.c -o error_handling.o 