#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include <inttypes.h>
#include <stddef.h>

struct memory_manager {
    static memory_manager* active_memory_manager;
    
    memory_manager(void* heap_start, uint64_t heap_size);

    ~memory_manager();

    void* kmalloc(size_t size);
    void free(void* pointer);

    static bool is_manager_active();

    void* operator new(size_t size);
    void* operator new[](size_t size);

    void operator delete(void* pointer);
    void operator delete[](void* pointer);
};

#endif