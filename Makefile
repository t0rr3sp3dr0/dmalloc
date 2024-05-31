HEAP_SIZE ?= 1048576

all: clean dmalloc.dylib
.PHONY: all

clean:
	rm -f ./dmalloc.dylib
.PHONY: clean

dmalloc.dylib: main.c
	$(CC) -DHEAP_SIZE=$(HEAP_SIZE) -o $@ -shared $^
