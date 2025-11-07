CFLAGS= -std=c99 -pedantic -Werror -Wall -Wextra -Wvla -fvisibility=hidden -fPIC
CPPFLAGS=-D_DEFAULT_SOURCE
LDFLAGS= -shared -Wl,--no-undefined

SRCS=src/malloc.c src/big_nmap/big_nmap.c src/bucket/bucket.c 

OBJS=$(SRCS:.c=.o)
library: libmalloc.so


libmalloc.so: $(OBJS)
	gcc $(LDFLAGS) -o libmalloc.so $(OBJS)

check:
	./tests/tests.sh

clean:
	$(RM) $(OBJS) libmalloc.so
