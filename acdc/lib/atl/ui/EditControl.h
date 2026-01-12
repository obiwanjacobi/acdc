#pragma once
#include <stdint.h>
#include "DisplayWriter.h"
#include "InputControl.h"
#include "UpDownControl.h"

/** The EditControl is a text edit control that allows a FixedString to be edited.
 *	\tparam StringT is a FixedString type that holds the string and implements:
 *  `typename ItemT`
 *  `ItemT GetAt(uint8_T)` (Array)
 *  `void SetAt(uint8_t, ItemT)` (FixedArray)
 *  `uint8_t getCapacity()` (Array).
 *	\tparam CharacterIteratorT is an Iterator that manages the characters that can be selected
 *  when editing the EditControl and implements:
 *  `bool MoveNext()` (Iterator)
 *  `bool MoveBack()` (Iterator)
 *  `bool MoveTo(char)` (Iterator)
 *  `char getCurrent() const` (Iterator).
 *	\tparam ValueT is the type used to receive the UpDownControl<ValueT> calls and implements:
 *  `const char* getText()`
 *  `void IncrmentValue()`
 *  `void DecrmentValue()`.
 *  These methods are implemented on EditControl itself.
 */
template <class StringT, class CharacterIteratorT, class ValueT>
class EditControl : public UpDownControl<ValueT>
{
    typedef UpDownControl<ValueT> BaseT;
    friend BaseT;

public:
    /** Constructs an initialized instance.
     *  \param str points to the string buffer of the FixedString that is being edited.
     *  \param iterator points to the Iterator that provides the characters during editing.
     *  \param valueThis is used by the UpDownControl to callback value manipulation on.
     *  \param pos is an optional position relative to its siblings.
     *  \param handler callback handler for custom nav-commands and input events.
     */
    EditControl(StringT *str, CharacterIteratorT *iterator, ValueT *valueThis, uint8_t pos = 0, InputControlHandler *handler = nullptr)
        : BaseT(valueThis, pos, handler), _iterator(iterator)
    {
        setString(str);
    }

    /** Retrieves the text the EditControl displays.
     *  Called by the UpDownControl to retrieve the 'value' to display.
     *  \return Returns the pointer to the text. Can be NULL.
     */
    const char *getText() const
    {
        return (const char *)_str;
    }

    /** Assigns the String the EditControl displays.
     *  \param text points to a string instance.
     *  The string is NOT copied and the same buffer is used for editing.
     */
    void setString(StringT *str)
    {
        _str = str;
        _editIndex = _str == nullptr ? InvalidEditIndex : 0;
        RepositionIterator();
    }

    /** Overridden to display the cursor on the character position that is being edited.
     *  \param output is used to output text and position the cursor.
     *  \mode indicates what to display.
     */
    void Display(DisplayWriter *output) override
    {
        const char *text = getText();
        if (text != nullptr)
            output->Display(text);
    }

    void DisplayCursor(DisplayWriter *output) override
    {
        if (BaseT::getIsSelected())
        {
            uint8_t col = BaseT::getPosition();
            if (_editIndex != InvalidEditIndex)
                col += _editIndex;
            output->GoTo(DisplayWriter::CurrentPos, col);
        }
    }

    /** Handles the `Left` and `Right` navigation commands to change editing position inside the string.
     *  \param navCmd is the navigation command.
     *  \return Returns true if the command was handled.
     */
    bool OnNavigationCommand(NavigationCommands navCmd) override
    {
        bool handled = false;

        switch (navCmd)
        {
        case NavigationCommands::Left:
            // LogTrace("Ed:Nav->L");
            handled = TryMoveCursorLeft();
            break;
        case NavigationCommands::Right:
            // LogTrace("Ed:Nav->R");
            handled = TryMoveCursorRight();
            break;
        default:
            break;
        }

        if (handled)
            return true;

        return BaseT::OnNavigationCommand(navCmd);
    }

    /** Attempts to move the editing cursor to the right.
     *  \return Returns true when successful (even when the end of the string was reached).
     */
    bool TryMoveCursorRight()
    {
        if (BaseT::getIsSelected() &&
            _editIndex != InvalidEditIndex)
        {
            if (_editIndex + 1 < getEditSize())
            {
                _editIndex++;
                RepositionIterator();
            }
            return true;
        }

        return false;
    }

    /** Attempts to move the editing cursor to the left.
     *  \return Returns true when successful (even when the start of the string was reached).
     */
    bool TryMoveCursorLeft()
    {
        if (BaseT::getIsSelected() &&
            _editIndex != InvalidEditIndex)
        {
            if (_editIndex > 0)
            {
                _editIndex--;
                RepositionIterator();
            }
            return true;
        }

        return false;
    }

protected:
    /** Called by the UpDownControl to increment the 'value' - char at edit position.
     */
    void IncrementValue()
    {
        if (_str != nullptr && _iterator->MoveNext())
        {
            // LogTrace("Ed:IncVal");
            _str->SetAt(_editIndex, _iterator->getCurrent());
        }
    }

    /** Called by the UpDownControl to decrement the 'value' - char at edit position.
     */
    void DecrementValue()
    {
        if (_str != nullptr && _iterator->MoveBack())
        {
            // LogTrace("Ed:DecVal");
            _str->SetAt(_editIndex, _iterator->getCurrent());
        }
    }

    /** Helper method that sets the CharacterIteratorT at the character at the current edit position.
     */
    void RepositionIterator()
    {
        if (_str != nullptr &&
            _editIndex >= 0 && _editIndex != InvalidEditIndex)
        {
            if (!_iterator->MoveTo(_str->GetAt(_editIndex)))
            {
                // MoveTo can fail when _str[_editIndex] returns a terminating \0
                _iterator->Reset();
                // TODO: Do we call (ValueT*)->IncrementValue() here?
                _iterator->MoveNext();
                // make sure there is a valid char (overwrite terminating \0)
                _str->SetAt(_editIndex, _iterator->getCurrent());
            }
        }
    }

    /** Overridden to manage the character edit position when going in/out of `Focused` and `Selected`.
     *  Does not alter behavior.
     *  \param newState is the proposed state.
     *  \return Returns true when the state change is allowed.
     */
    bool BeforeChangeState(ControlState newState) override
    {
        if (!BaseT::BeforeChangeState(newState))
            return false;

        if (newState == ControlState::Selected)
        {
            _editIndex = 0;
            RepositionIterator();
        }

        return true;
    }

private:
    static const uint8_t InvalidEditIndex = 0xFF;

    StringT *_str;
    CharacterIteratorT *_iterator;
    uint8_t _editIndex;

    uint8_t getEditSize() const
    {
        return _str->getCapacity() - 1;
    }
};