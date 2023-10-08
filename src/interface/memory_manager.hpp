#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include <inttypes.h>
#include <stddef.h>

#define MIN_CHUNK_SIZE 0

// My Memory Manager is shamelessly stolen from the Following GitHub Repo:
//      https://github.com/cmilatinov/sick-os-64/tree/master
// From this some Parts have been changed
//
// This is due to the Fact that i currently do not have the time or knowledge required to implement/understand my completly own Memory Manager, but want to continue the Project
// Wrting my own manager would take a lot of research and Time. Maybe i will be back later. 
// TODO -> Check why CTors and DTors are not called

struct memory_manager {
    private:
        // Memory chunk struct inserted before every memory chunk
        // Two unallocated memory chunks one next to another should
        // never occur, as they should be merged when deallocated
        struct memory_chunk {
            memory_chunk* prev;
            memory_chunk* next;
            uint8_t allocated;
            size_t size;
        } __attribute__((packed));

        // Pointer to first memory chunk in heap
        memory_chunk* first;

        // Merge the target chunk with the one to its left
        inline void merge_left(memory_chunk* toDelete);

        // The active memory manager, used for memory allocation 
        // without needing a reference to the manager
        static memory_manager* active_memory_manager;

    public:        
        memory_manager(void* heap_start, uint64_t heap_size);

        ~memory_manager();

        void* my_kmalloc(size_t size);
        void my_kfree(void* pointer);

        static bool is_manager_active();

        void* operator new(size_t size);
        void* operator new[](size_t size);

        void operator delete(void* pointer);
        void operator delete[](void* pointer);

        friend void* my_kmalloc(size_t size);
        friend void my_kfree(void* ptr);
};

//** Referenceless Implementations of KMalloc and Kfree 
inline void* my_kmalloc(size_t size) {
    if(memory_manager::is_manager_active())
        return memory_manager::active_memory_manager->my_kmalloc(size);
    return nullptr;
}

inline void my_kfree(void * ptr) {
    if(memory_manager::is_manager_active())
        return memory_manager::active_memory_manager->my_kfree(ptr);
}

void kmemcpy(const void * src, void * dest, size_t size);
void kmemcpy_f(const void * src, void * dest, size_t size);

void* kmemset(void * dest, uint8_t src, size_t c);
void kmemset_f(void * dest, size_t size, uint8_t value);

#endif