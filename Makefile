.PHONY: clean

CC := gcc
LD := ld
CPPFLAGS := -I/usr/local/opt/readline/include -I./include
LDFLAGS := -lreadline -L/usr/local/opt/readline/lib

CFILES := $(shell find src/ -name "*.c")
OBJS := $(CFILES: .c=.o)
prog = cinter
obj = $(src:%.c=%.o)

run: $(src)
	$(CC) $(CFILES) -o $(prog) $(LDFLAGS) $(CPPFLAGS)
	./$(prog)

clean:
	rm $(obj) $(prog) *~
