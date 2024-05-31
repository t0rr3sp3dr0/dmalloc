#include <malloc/malloc.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static char heap_head[HEAP_SIZE] = { 0 };
static char *heap_tail = heap_head;

static size_t _Malloc_good_size(size_t size) {
    return size + ((_Alignof(max_align_t) - size) % _Alignof(max_align_t));
}

static size_t _Malloc_size(const void *ptr) {
    size_t aligned_size_t_size = _Malloc_good_size(sizeof(size_t));
    return *((size_t *) (ptr - aligned_size_t_size));
}

static void _Free(void *ptr) {
    (void) ptr;
}

static void *_Malloc(size_t size) {
    size_t aligned_size_t_size = _Malloc_good_size(sizeof(size_t));
    size_t aligned_size = _Malloc_good_size(size);

    if (heap_tail + aligned_size_t_size + aligned_size > heap_head + HEAP_SIZE) {
        raise(SIGABRT);

        return NULL;
    }

    *((size_t *) heap_tail) = size;

    void *ptr = heap_tail + aligned_size_t_size;

    heap_tail = ptr + aligned_size;

    return ptr;
}

static void *_Calloc(size_t count, size_t size) {
    void *ptr = _Malloc(count * size);
    return memset(ptr, 0, _Malloc_size(ptr));
}

static void *_Realloc(void *old_ptr, size_t new_size) {
    if (!old_ptr) {
        return _Malloc(new_size);
    }

    size_t old_size = _Malloc_size(old_ptr);
    if (new_size <= old_size) {
        if (new_size < old_size) {
            size_t aligned_size_t_size = _Malloc_good_size(sizeof(size_t));
            *((size_t *) (old_ptr - aligned_size_t_size)) = new_size;
        }

        return old_ptr;
    }

    void *new_ptr = _Malloc(new_size);
    return memcpy(new_ptr, old_ptr, old_size);
}

static void *_Reallocf(void *ptr, size_t size) {
    return _Realloc(ptr, size);
}

static void *_Valloc(size_t size) {
    raise(SIGABRT);

    return _Malloc(size);
}

static void *_Aligned_alloc(size_t alignment, size_t size) {
    raise(SIGABRT);

    (void) alignment;

    return _Malloc(size);
}

struct interposer {
    void *dst;
    void *src;
};

__attribute__((used)) static struct interposer interposers[] __attribute__((section("__DATA,__interpose"))) = {
    { _Free, free },
    { _Malloc, malloc },
    { _Calloc, calloc },
    { _Realloc, realloc },
    { _Reallocf, reallocf },
    { _Valloc, valloc },
    { _Aligned_alloc, aligned_alloc },
    { _Malloc_good_size, malloc_good_size },
    { _Malloc_size, malloc_size },
};
