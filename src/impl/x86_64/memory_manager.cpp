#include "memory_manager.hpp"

memory_manager* memory_manager::active_memory_manager = nullptr;
memory_manager::memory_chunk* memory_manager::first = nullptr;

memory_manager::memory_manager(void* heapStart, uint64_t heapSize) {
    // Create initial chunk
    first = reinterpret_cast<memory_chunk*>(heapStart);
    first->prev = nullptr;
    first->next = nullptr;
    first->allocated = false;
    first->size = heapSize - sizeof(memory_chunk);
    
    // Set this manager as the active manager
    active_memory_manager = this;
}

memory_manager::~memory_manager() {
    if(active_memory_manager == this) 
        active_memory_manager = nullptr;
}

// Implementation of the Buddy Allocators KMalloc
void* memory_manager::my_kmalloc(size_t size) {
    // Can't create empty memory chunks
    if(!size) {
        io::my_cout << "Cant create empty memory chunks..." << io::OSTREAM_APPEND::endl;
        return nullptr;
    }

    memory_chunk* mem = first;
    if(DEBUG_MM) io::my_cout << "Size: " << mem->size << " allocated... " << mem->allocated <<"\n";
    do {
        if(DEBUG_MM) io::my_cout << "next Chunk would be..." << (mem->next != nullptr) << io::OSTREAM_APPEND::endl;   
        // Memory chunk is big enough and not allocated
        if(mem->size >= size && !mem->allocated) {
            // Memory chunk is not big enough to be split in two
            // so just allocate the full chunk
            if(mem->size <= (size + sizeof(memory_chunk) + MIN_CHUNK_SIZE)) {
                mem->allocated = true;
            // Split chunk in two and allocate first one
            } else {
                // Pointer to next chunk = this chunk + chunk header size + needed size 
                memory_chunk* next = reinterpret_cast<memory_chunk*>(reinterpret_cast<uint8_t*>(mem) + sizeof(memory_chunk) + size);

                // Insert next chunk
                next->prev = mem;
                next->next = mem->next;
                next->allocated = false;
                next->size = mem->size - size - sizeof(memory_chunk);

                // Resize this chunk and setup pointer to next chunk
                mem->size = size;
                mem->next = next;
                mem->allocated = true;
                if(DEBUG_MM) io::my_cout << "Set the next pointer to..." << (mem->next != nullptr) << "  " << (first->next != nullptr) << "\n";
                if(DEBUG_MM) io::my_cout << "Size of this: " << mem->size << " Allocated this "<< mem->allocated << "Size of next: " << next->size << " Allocated next "<< next->allocated <<"\n";
            }
            memory_chunk* test = reinterpret_cast<memory_chunk*>(reinterpret_cast<uint8_t*>(mem) + sizeof(memory_chunk) + size);
            if(DEBUG_MM) io::my_cout << "and tests size and next..." << test->size << "      " << (test != nullptr) << " f " << (first->next != nullptr) <<io::OSTREAM_APPEND::endl;
            return reinterpret_cast<uint8_t*>(mem) + sizeof(memory_chunk);
        }

    } while((mem = mem->next) != nullptr);

    if(DEBUG_MM) io::my_cout << "KMALLOC ELSE... Thats probably bad... " << io::OSTREAM_APPEND::endl;

    return nullptr;
}

void memory_manager::my_kfree(void* ptr) {
    // Free the Memorychunk including its header 
    memory_chunk* mem = reinterpret_cast<memory_chunk*>(reinterpret_cast<uint8_t*>(ptr) - sizeof(memory_chunk));

    // should NOT free Memory that is not allocated
    if(mem->allocated != true) {
        io::my_cout << "Forbidden Free..." << io::OSTREAM_APPEND::endl;
        return;
    }

    // If at all possible Merge the empty Chunks together to free the Memory
    // Merge left
    if(mem->prev != nullptr && !mem->prev->allocated)
        merge_left(mem);

    // Merge right
    if(mem->next != nullptr && !mem->next->allocated) 
        merge_left(mem->next);

    // Actually FREE the Memory
    // TODO -> Check this FIX
    /** DO NOT FORGET **/
    /*****REALLY NOT*****/
    //** Seems it is appropriate, but TODO Marker left in case of nasty future errors in MM **/
    mem->allocated = false;     
}

inline void memory_manager::merge_left(memory_chunk* toDelete) {
    // Update left pointer
    toDelete->prev->next = toDelete->next;

    // Update right pointer if not last chunk in list
    if(toDelete->next != nullptr) 
        toDelete->next->prev = toDelete->prev;

    // Add this chunk's full size to the left chunk
    toDelete->prev->size += sizeof(memory_chunk) + toDelete->size;

}

bool memory_manager::is_manager_active() {
    return (active_memory_manager != nullptr);
}

void* operator new(size_t size) {
    return my_kmalloc(size);
}

void* operator new[](size_t size) {
    return my_kmalloc(size);
}

void operator delete(void* ptr) {
    my_kfree(ptr);
}

void operator delete(void* ptr, size_t size) {
    my_kfree(ptr);
}

void operator delete[](void* ptr) {
    my_kfree(ptr);
}

void operator delete[](void* ptr, size_t size) {
    my_kfree(ptr);
}

void kmemcpy(const void* src, void* dest, size_t size) {
    uint8_t* destPtr = reinterpret_cast<uint8_t*>(dest);
    const uint8_t* srcPtr = reinterpret_cast<const uint8_t*>(src);
    
    for(size_t i = 0; i < size; i++) {
        destPtr[i] = srcPtr[i];
    }
}

void kmemcpy_f(const void* src, void* dest, size_t size) {
    uint64_t* destPtr = reinterpret_cast<uint64_t*>(dest);
    const uint64_t* srcPtr = reinterpret_cast<const uint64_t*>(src);
    
    for(size_t i = 0; i < size / sizeof(uint64_t); i++) {
        destPtr[i] = srcPtr[i];
    }

    if(size & 0b111) {
        uint8_t* d = reinterpret_cast<uint8_t*>(destPtr) + size - (size & 0b111);
        const uint8_t* s = reinterpret_cast<const uint8_t*>(srcPtr) + size - (size & 0b111);

        for(uint8_t i = 0; i < (size & 0b111); i++) {
            d[i] = s[i];
        }
    }
}

void* kmemset(void* dest, uint8_t src, size_t c) {
    void* temp = dest;
    __asm__ volatile (
        "rep stosb"
        : "=D"(dest), "=c"(c)
        : "0"(dest), "a"(src), "1"(c)
        : "memory"
    );
    return temp;
}

void kmemset_f(void* dest, size_t size, uint8_t value) {
    uint64_t* destPtr = reinterpret_cast<uint64_t*>(dest);
    uint8_t _value64[8] = { value, value, value, value, value, value, value, value };
    uint64_t value64 = *reinterpret_cast<uint64_t*>(_value64);

    size_t end = size / sizeof(uint64_t);
    for(size_t i = 0; i < end; i++) {
        destPtr[i] = value64;
    }
    
    size_t leftover = size & 0x7;
    if(leftover) {
        uint8_t* d = reinterpret_cast<uint8_t*>(destPtr + end);

        for(uint8_t i = 0; i < leftover; i++) {
            d[i] = value;
        }
    }
}

void memory_manager::print_size_chunk() const {
    int iteration_number = 0;
    // Init with negative Header Size, as there is always an unavoidable Header so it is not really a direct loss...
    int loss_due_to_header = -sizeof(memory_chunk);
    memory_chunk* iterator = this->first;
    do {
        if(iterator != nullptr) {
            io::my_cout << (++iteration_number) << ". Chunk's Size is currently: " << iterator->size << io::OSTREAM_APPEND::endl;
            loss_due_to_header += sizeof(memory_chunk);
        }
    } while((iterator = iterator->next) != nullptr);
    io::my_cout << "Loss due to Header-Size: " << loss_due_to_header << "\n";
}