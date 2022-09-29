a:
	clang -lpthread threads.c 
	./a.out
b:
	clang -lpthread -std=gnu11 -g -fsanitize=thread threads.c
	./a.out
c:
	clang -lpthread -lrt -std=gnu11 -g -fsanitize=thread taskc.c
	./a.out
