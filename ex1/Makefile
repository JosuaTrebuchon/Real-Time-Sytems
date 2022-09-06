# program executable name
TARGET = time_test

# co;piler flags
CFLAGS = -g -Wall -std=gnu11

# linker flags
LDFLAGS = -g

# list of sources
SOURCES = $(shell find -name "*.c")

# default rul, to co;pile everything
all: $(TARGET)

# define object files
OBJECTS = $(SOURCES:.c=.o)

# link programs
$(TARGET): $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^

# compile
%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

#cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)

	







