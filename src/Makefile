bintap: bintap.c opts.o tapfile.o basic.o
	$(CC) $(CFLAGS) -o $@ $^

bintap.c: opts.h tapfile.h basic.h
opts.c: opts.h
tapfile.c: tapfile.h
basic.c: basic.h

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	$(RM) opts.o tapfile.o basic.o bintap
