#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cstddef>
#include "io.hpp"

inline void assert(bool con) {
    if(con)
        io::my_cout << "Assert returned True";
    else
        io::my_cout << "ASSERT FAILED!\n";
}

template<typename T>
class Vector {
    public:
        Vector();
        ~Vector();

        void pushBack(const T& value);
        const T& operator[](size_t index) const;
        T& operator[](size_t index);
        size_t size() const;

    private:
        void resize(size_t newCapacity);

        T* data;
        size_t capacity;
        size_t length;
};

template<typename T>
Vector<T>::Vector() : data(nullptr), capacity(0), length(0) {

}

template<typename T>
Vector<T>::~Vector() {
    delete[] data;
}

template<typename T>
void Vector<T>::pushBack(const T& value) {
    if(length == capacity)
        resize(capacity == 0 ? 1 : capacity * 2);
    data[length++] = value;
}

template<typename T>
const T& Vector<T>::operator[](size_t index) const {
    //assert(index < length);
    return data[index];
}

template<typename T>
T& Vector<T>::operator[](size_t index) {
    // assert(index < length);
    return data[index];
}

template<typename T>
size_t Vector<T>::size() const {
    return length;
}

template<typename T>
void Vector<T>::resize(size_t newCapacity) {
    T* newData = static_cast<T*>(operator new[](newCapacity * sizeof(T)));
    for(size_t i = 0; i < length; ++i)
        newData[i] = data[i];
    if(data)
        delete[] data;
    data = newData;
    capacity = newCapacity;
}

#endif