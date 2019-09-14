src = main.c edit.c operate.c magic.c
prog = test
obj = $(src:%.c=%.o)

run: $(src)
	gcc $(src) -o $(prog)
	./$(prog)

clean:
	rm -rf $(obj) $(prog) cinter.c *~
