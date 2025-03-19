#pragma once
#include <stdint.h>
#include "DefaultOfT.h"

template <typename T>
class Slice
{
public:
    Slice(T *value, uint8_t length)
        : _value(value), _length(length)
    {
    }

    T &operator[](uint8_t index)
    {
        if (!IsValidIndex(index))
            return Default<T>::DefaultOfT;

        return _value[index];
    }

    const T &operator[](uint8_t index) const
    {
        if (!IsValidIndex(index))
            return Default<T>::DefaultOfT;

        return _value[index];
    }

    const T *getValue() const
    {
        return _value;
    }
    uint8_t getLength() const
    {
        return _length;
    }

    bool IsValidIndex(uint8_t index) const
    {
        return index < _length;
    }

private:
    T *_value;
    uint8_t _length;
};
