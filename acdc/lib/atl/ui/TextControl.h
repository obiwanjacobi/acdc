#pragma once
#include "../FixedString.h"
#include "../Iterator.h"
#include "EditControl.h"

/** The TextControl is a text edit control that allows a FixedString to be edited.
 *	\tparam StringT is a FixedString type that holds the string and implements
 *  `typename ItemT`
 *  `ItemT GetAt(uint8_T)` (Array)
 *  `void SetAt(uint8_t, ItemT)` (FixedArray)
 *  `uint8_t getCapacity()` (Array).
 *	\tparam CharacterIteratorT is an Iterator that manages the characters that can be selected
 *  when editing the TextControl and implements:
 *  `bool MoveNext()` (Iterator)
 *  `bool MoveBack()` (Iterator)
 *  `bool MoveTo(char)` (Iterator)
 *  `char getCurrent() const` (Iterator).
 */
template <class StringT, class CharacterIteratorT>
class TextControl : public EditControl<StringT, CharacterIteratorT, TextControl<StringT, CharacterIteratorT>>
{
    typedef EditControl<StringT, CharacterIteratorT, TextControl<StringT, CharacterIteratorT>> BaseT;
    friend BaseT;

public:
    /** Constructs an initialized instance.
     *  \param str points to the string buffer of the FixedString that is being edited.
     *  \param iterator points to the Iterator that provides the characters during editing.
     *  \param pos is an optional position relative to its siblings.
     *  \param handler callback handler for custom nav-commands and input events.
     */
    TextControl(StringT *str, CharacterIteratorT *iterator, uint8_t pos = 0, InputControlHandler *handler = nullptr)
        : BaseT(str, iterator, this, pos, handler) {}
};

template <const char StartChar, const char EndChar>
class CharRangeProvider
{
public:
    typedef char ItemT;

    ItemT getDefaultItem() const
    {
        return StartChar;
    }

    int8_t IndexOf(char item) const
    {
        return (int8_t)item - StartChar;
    }

    char GetAt(int8_t index) const
    {
        return (char)StartChar + index;
    }

    int8_t getCount() const
    {
        return EndChar - StartChar;
    }
};

// from space ' ' till end of ascii (except 127:DEL)
typedef WrapAroundIterator<CharRangeProvider<0x20, 0x7E>> AsciiCharIterator;

template <const uint8_t Size>
class AsciiTextControl : public TextControl<FixedString<Size + 1>, AsciiCharIterator>
{
    typedef TextControl<FixedString<Size + 1>, AsciiCharIterator> BaseT;
    friend BaseT;

public:
    /** Constructs a new instance.
     *  \param pos is an optional position relative to its siblings.
     *  \param handler callback handler for custom nav-commands and input events.
     */
    AsciiTextControl(uint8_t pos = 0, InputControlHandler *handler = nullptr)
        : BaseT(&_buffer, &TextIterator, pos, handler) {}

private:
    FixedString<Size + 1> _buffer;

    // one static instance
    static AsciiCharIterator TextIterator;
};

template <const uint8_t Size>
AsciiCharIterator AsciiTextControl<Size>::TextIterator;