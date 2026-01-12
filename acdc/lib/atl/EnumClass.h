#pragma once

// Macro to enable bitwise ops for enum class types
// Add this macro to the 'enum class' definition to implement |&~operators.
#define ENUM_FLAG(EnumType)                                                                    \
    inline constexpr EnumType operator|(EnumType lhs, EnumType rhs)                            \
    {                                                                                          \
        return static_cast<EnumType>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs)); \
    }                                                                                          \
    inline constexpr EnumType operator&(EnumType lhs, EnumType rhs)                            \
    {                                                                                          \
        return static_cast<EnumType>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs)); \
    }                                                                                          \
    inline constexpr EnumType operator~(EnumType e)                                            \
    {                                                                                          \
        return static_cast<EnumType>(~static_cast<unsigned>(e));                               \
    }

template <typename EnumType>
inline constexpr EnumType Or(EnumType lhs, EnumType rhs)
{
    return static_cast<EnumType>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

template <typename EnumType>
inline constexpr EnumType And(EnumType lhs, EnumType rhs)
{
    return static_cast<EnumType>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
}

template <typename EnumType>
inline constexpr EnumType Not(EnumType e)
{
    return static_cast<EnumType>(~static_cast<unsigned>(e));
}

template <typename EnumClassT>
bool HasFlag(EnumClassT value, EnumClassT flag)
{
    return And(value, flag) == flag;
}

template <typename EnumClassT>
bool HasAnyFlags(EnumClassT value, EnumClassT flag1, EnumClassT flag2)
{
    return And(value, And(flag1, flag2)) != 0;
}
template <typename EnumClassT>
bool HasAnyFlags(EnumClassT value, EnumClassT flag1, EnumClassT flag2, EnumClassT flag3)
{
    return And(value, And(flag1, And(flag2, flag3))) != 0;
}
