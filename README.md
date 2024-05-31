# dmalloc
Dummy Malloc Interposer Implementation

## Build

```bash
make HEAP_SIZE='8388608'
```

## Use

```bash
DYLD_INSERT_LIBRARIES='./dmalloc.dylib' ./a.out
```
