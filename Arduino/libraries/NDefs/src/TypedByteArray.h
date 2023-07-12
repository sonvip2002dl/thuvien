#ifndef TypedByteArray_h
#define TypedByteArray_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

template<typename T, size_t size>
struct TypedByteArray
{
    union
    {
        byte bytes[size];
        T data;
    };

    TypedByteArray()
    {
    }

    TypedByteArray(const byte *value)
    {
        memmove(bytes, value, size);
    }

    TypedByteArray(T &l)
        : data(l)
    {
    }

    TypedByteArray(T &&r)
        : data(r)
    {
    }

    operator byte *()
    {
        return bytes;
    }

    operator T()
    {
        return data;
    }

    void operator=(const byte *v)
    {
        memmove(bytes, v, size);
    }

    void operator=(const T &l)
    {
        data = l;
    }

    void operator=(const T &&r)
    {
        data = r;
    }
};

#endif