#ifndef BUFFER_PD
#define BUFFER_PD

#include <cstdlib>
#include <algorithm>
#include <stdexcept>

template<typename T>
class Buffer
{
private:
    T* buffer;
    const std::size_t size;
public:
    Buffer() = delete;
    Buffer(std::size_t size) : size(size)
    {
        this->buffer = new T[size];
    }
    ~Buffer()
    {
        delete[] this->buffer;
    }

    void insert(T element)
    {
        std::rotate(this->buffer,this->buffer+1,this->buffer+this->size);
        this->buffer[0] = element;
    }

    T at(std::size_t ind) const
    {
        if(ind >= size or ind < 0)
            throw std::invalid_argument("Index out of range");
        return this->buffer[ind];
    }

    T operator[](std::size_t ind) const
    {
        if(ind >= size or ind < 0)
            throw std::invalid_argument("Index out of range");
        return this->buffer[ind];
    }

    T* begin() const noexcept { return this->buffer; }

    T* end() const noexcept { return this->buffer + this->size; }
};

#endif