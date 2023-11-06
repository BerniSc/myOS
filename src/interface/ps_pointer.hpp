#ifndef DS_POINTER_HPP
#define DS_POINTER_HPP

/**
 * Implementation of a Pseudo-Smart Pointer in the C++ Kernel. It will not guarantee anything like Uniqueness etc.,
 * but it will enusre that the Memory Management is done properly and the Memory is freed once the variable is out of Scope. 
 * 
 * This is achieved by Capsulating the Object that has been allocated using "new" in a staticly created variable that gets cleaned up once its 
 * Scope is left and in its Destructor the Memory is then freed correctly.
 * 
 * For this to Work this class _must_ recieve dynamicly allocated Memory via ptr!!!
*/
template<typename T>
class ps_pointer {
    private:
        T* stored_ptr;
    public:
        explicit ps_pointer(T* ptr) : stored_ptr(ptr) {
        
        }

        ~ps_pointer() {
            if(this->stored_ptr != nullptr)
                delete this->stored_ptr;
        }

        T& operator*() {
            return *this->stored_ptr;
        }

        T* operator->() {
            return this->stored_ptr;
        }
};

#endif