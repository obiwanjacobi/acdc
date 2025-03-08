#pragma once
#include <stdint.h>

/** Manipulates a single bit in a variable.
 *  Bit is a static class, meaning that no instance can be constructed.
 *  All methods are called like `Bit<2>::Set(myVar);` which would set bit2 in myVar.
 *  Use BitFlag instead of Bit when the bitIndex is dynamic/only known at runtime.
 *  Bit is slightly more optimal when the bitIndex is known at compile time (hard-coded).
 *  \tparam BitIndex is the zero-based index where 0 is the least significant bit.
 */
template <const uint8_t BitIndex>
class Bit
{
public:
    /** Sets (true) the BitIndex bit in the target variable.
     *  \tparam T the data type of the target variable.
     *  \param target the variable that will be changed.
     */
    template <typename T>
    inline static void Set(T &target)
    {
        target |= getMask<T>();
    }

    /** Sets the BitIndex bit in the target variable to the specified value.
     *  \tparam T the data type of the target variable.
     *  \param target the variable that will be changed.
     *  \param value specifies the value for the bit at BitIndex.
     */
    template <typename T>
    inline static void Set(T &target, bool value)
    {
        T mask = getMask<T>();

        // clear bit
        target &= ~mask;

        if (value)
        {
            // set bit
            target |= mask;
        }
    }

    /** Resets (false) the BitIndex bit in the target variable.
     *  \tparam T the data type of the target variable.
     *  \param target the variable that will be changed.
     */
    template <typename T>
    inline static void Reset(T &target)
    {
        // clear bit
        target &= ~getMask<T>();
    }

    /** Toggles (inverts) the BitIndex bit in the target variable.
     *  \tparam T the data type of the target variable.
     *  \param target the variable that will be changed.
     */
    template <typename T>
    inline static void Toggle(T &target)
    {
        target ^= getMask<T>();
    }

    /** Indicates if the BitIndex bit in the target variable is a one (true).
     *  \tparam T the data type of the target variable.
     *  \param target the variable that contains the bit.
     */
    template <typename T>
    inline static bool IsTrue(T target)
    {
        return (target & getMask<T>()) > 0;
    }

    /** Indicates if the BitIndex bit in the target variable is a zero (false).
     *  \tparam T the data type of the target variable.
     *  \param target the variable that contains the bit.
     */
    template <typename T>
    inline static bool IsFalse(T target)
    {
        return (target & getMask<T>()) == 0;
    }

protected:
    /** Calculates the bit mask for the BitIndex bit.
     *  \tparam T the data type of the target variable.
     *  \return Returns a value with the BitIndex'th bit set.
     */
    template <typename T>
    inline static T getMask()
    {
        static const T mask = 1 << BitIndex;
        return mask;
    }

private:
    Bit()
    {
    }
};