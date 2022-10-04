a:
	clang -lpthread threads.c 
	./a.out
b:
	clang -lpthread -std=gnu11 -g -fsanitize=thread threads.c
	./a.out
c:
	clang -lpthread -lrt -std=gnu11 -g -fsanitize=thread taskc.c
	./a.out
d:
	clang -lpthread -std=gnu11 UnboundedPriorityInversion.c
e:
	clang -lpthread -std=gnu11 UnboundedPriorityInversionE.c
f:
	clang -s double_checked.c -o double_checked -std=gnu11 -g -O3 -lpthread
g:
	clang -s double_checked.c -o double_checked -std=gnu11 -g -O3 -lpthread -fsanitize=thread
h: 
	clang -s philosophers.c -o philosophers -std=gnu11 -g -O3 -lpthread -fsanitize=thread