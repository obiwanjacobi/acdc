#pragma once
#include <stdint.h>
#include "SelectControl.h"

template <class OptionsT>
class OptionsIterator
{
public:
    typedef const char *ItemT;

    OptionsIterator(const void *options)
    {
        _options = (OptionsT *)options;
    }

    const char *getDefaultItem() const
    {
        return _options->GetAt(0);
    }

    int8_t IndexOf(ItemT item) const
    {
        return _options->IndexOf(item);
    }

    ItemT GetAt(int8_t index) const
    {
        return _options->GetAt(index);
    }

    int8_t getCount() const
    {
        return _options->getCount();
    }

private:
    OptionsT *_options;
};

/** The OptionsControl is an edit control that allows selecting an option from a list.
 *	\tparam OptionsT represents the options type that contains the option text to display and implements:
 *  `const char* GetAt(int8_t)` (Array)
 *  `int8_t IndexOf(ItemT)` (Array)
 *  `int8_t getCount()` (Array)
 */
template <class OptionsT>
class OptionsControl : public SelectControl<Iterator<OptionsIterator<OptionsT>>, OptionsControl<OptionsT>>
{
public:
    typedef SelectControl<Iterator<OptionsIterator<OptionsT>>, OptionsControl<OptionsT>> BaseT;
    friend BaseT;

    /** Constructs an initialized instance.
     *  \param options (pointer to) the options (of type OptionsT).
     *  \param pos is an optional position relative to its siblings.
     *  \param handler callback handler for custom nav-commands and input events.
     */
    OptionsControl(OptionsT *options, uint8_t pos = 0, InputControlHandler *handler = nullptr)
        : BaseT(&_iterator, this, pos, handler), _iterator((const void *)options) {}

private:
    Iterator<OptionsIterator<OptionsT>> _iterator;
};
