#pragma once
#include <stdint.h>

/** Math provides math functions.
 *  The Math class is a static class and cannot be instantiated.
 */
class Math
{
public:
    /** Returns the absolute value.
     *  \tparam T is the data type of the value.
     *  \param value is the value to return the non-negative representation for.
     *  \return Returns the non-negative value.
     */
    template <typename T>
    static T Abs(T value)
    {
        return value >= 0 ? value : -value;
    }

private:
    Math() {}
};