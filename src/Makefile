src = main.c edit.c operate.c command.c magic.c
prog = cinter
obj = $(src:%.c=%.o)

LDFLAGS="-L/usr/local/opt/readline/lib"
CPPFLAGS="-I/usr/local/opt/readline/include"

run: $(src)
	gcc $(src) -o $(prog) -lreadline
	./$(prog)

clean:
	rm -rf $(obj) $(prog) *~
