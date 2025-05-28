CC = gcc
CFLAGS = -ansi -Wall -pedantic
GEN_DEPS = lists.h errors.h whitespaces.h staements.h

EXE_DEPS = assembler.o lists.o instructions.o errors.o compilation.o pass_one.o pass_two.o pre_assembler.o statements.o whitespaces.o

assembler: $(EXE_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

assembler.o: assembler.c pre_assembler.o pass_one.o pass_two.o compilation.o
	$(CC) -c assembler.c $(CFLAGS) -o $@

lists.o: lists.c lists.h
	$(CC) -c lists.c $(CFLAGS) -o $@

instructions.o: instructions.c instructions.h
	$(CC) -c instructions.c $(CFLAGS) -o $@

errors.o: errors.c errors.h
	$(CC) -c errors.c $(CFLAGS) -o $@

compilation.o: compilation.c compilation.h
	$(CC) -c compilation.c $(CFLAGS) -o $@

pass_one.o: pass_one.c pass_one.h
	$(CC) -c pass_one.c $(CFLAGS) -o $@

pass_two.o: pass_two.c pass_two.h
	$(CC) -c pass_two.c $(CFLAGS) -o $@

pre_assembler.o: pre_assembler.c pre_assembler.h
	$(CC) -c pre_assembler.c $(CFLAGS) -o $@

statements.o: statements.c statements.h
	$(CC) -c statements.c $(CFLAGS) -o $@

whitespaces.o: whitespaces.c whitespaces.h
	$(CC) -c whitespaces.c $(CFLAGS) -o $@

clean:
	rm -rf *.o