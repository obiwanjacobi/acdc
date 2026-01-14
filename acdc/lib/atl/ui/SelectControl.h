#pragma once
#include <stdint.h>
#include "DisplayWriter.h"
#include "InputControl.h"
#include "UpDownControl.h"

/** The SelectControl is a edit control that selects an option from a list.
 *	\tparam OptionIteratorT is an Iterator that manages the options that can be selected
 *  when editing the SelectControl and implements:
 *  `bool MoveNext()` (Iterator)
 *  `bool MoveBack()` (Iterator)
 *  `bool MoveTo(const char*)` (Iterator)
 *  `bool MoveToIndex(int8_t)` (Iterator)
 *  `int8_t getCurrentIndex() const` (Iterator).
 *  `const char* getCurrent() const` (Iterator).
 *	\tparam ValueT is the type used to receive the UpDownControl<ValueT> calls and implements:
 *  `const char* getText()`
 *  `void IncrmentValue()`
 *  `void DecrmentValue()`.
 *  These methods are implemented on SelectControl itself.
 */
template <class OptionsIteratorT, class ValueT>
class SelectControl : public UpDownControl<ValueT>
{
public:
    typedef UpDownControl<ValueT> BaseT;
    friend BaseT;

    /** Constructs an initialized instance.
     *  \param iterator points to the Iterator that provides the options that can be selected.
     *  \param valueThis is used by the UpDownControl to callback value manipulation on.
     *  \param pos is an optional position relative to its siblings.
     *  \param handler callback handler for custom nav-commands and input events.
     */
    SelectControl(OptionsIteratorT *iterator, ValueT *valueThis, uint8_t pos = 0, InputControlHandler *handler = nullptr)
        : BaseT(valueThis, pos, handler), _iterator(iterator) {}

    /** Retrieves the text the TextControl displays.
     *  Called by the UpDownControl to retrieve the 'value' to display.
     *  \return Returns the pointer to the text. Can be NULL.
     */
    const char *getText() const
    {
        return _iterator->getCurrent();
    }
    bool TrySetText(const char *text)
    {
        return _iterator->MoveTo(text);
    }

    /** Retrieves the selected value.
     *  \return Returns the selected value. Can be invalid.
     */
    int8_t getValue() const
    {
        return _iterator.getCurrentIndex();
    }
    bool TrySetValue(int8_t value)
    {
        return _iterator.MoveToIndex(value);
    }

    /** Overridden to display the cursor on the character position that is being edited.
     *  \param output is used to output text and position the cursor.
     *  \mode indicates what to display.
     */
    // void Display(DisplayWriter *output) override
    // {
    //     const char *text = getText();
    //     if (text != nullptr)
    //         output->Display(text);
    // }

protected:
    /** Called by the UpDownControl to increment the 'value'.
     */
    void IncrementValue()
    {
        if (_iterator->MoveNext())
        {
            // LogTrace("Sel:IncVal");
        }
    }

    /** Called by the UpDownControl to decrement the 'value'.
     */
    void DecrementValue()
    {
        if (_iterator->MoveBack())
        {
            // LogTrace("Sel:DecVal");
        }
    }

    /** Helper method that sets the CharacterIteratorT at the character at the current edit position.
     */
    // void RepositionIterator()
    // {
    //     if (!_iterator->MoveToIndex(_selectIndex))
    //     {
    //         // MoveTo can fail when _str[_editIndex] returns a terminating \0
    //         _iterator->Reset();
    //         // TODO: Do we call (ValueT*)->IncrementValue() here?
    //         _iterator->MoveNext();
    //         // make sure there is a valid char (overwrite terminating \0)
    //         _str->SetAt(_editIndex, _iterator->getCurrent());
    //     }
    // }

    /** Overridden to manage the character edit position when going in/out of `Focused` and `Selected`.
     *  Does not alter behavior.
     *  \param newState is the proposed state.
     *  \return Returns true when the state change is allowed.
     */
    // bool BeforeChangeState(ControlState newState) override
    // {
    //     if (!BaseT::BeforeChangeState(newState))
    //         return false;

    //     if (newState == ControlState::Selected)
    //         RepositionIterator();

    //     return true;
    // }

private:
    OptionsIteratorT *_iterator;
};