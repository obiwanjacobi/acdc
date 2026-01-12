#pragma once
#include "DisplayWriter.h"
#include "InputControl.h"

/** The UpDownControl edits a value with the `Up` and `Down` commands.
 *  Management of the value itself is outside this class.
 *  \tparam ValueT represents the type that controls the value and implements:
 *  `const char* getText()`
 *  `void IncrmentValue()`
 *  `void DecrmentValue()`.
 */
template <class ValueT>
class UpDownControl : public InputControl
{
    typedef InputControl BaseT;

public:
    /** Constructs an initialized instance.
     *  \param value points to the value instance. Must not be NULL.
     *  \param pos is the optional position relative to its siblings.
     *  \param handler callback handler for custom nav-commands and input events.
     */
    UpDownControl(ValueT *value, uint8_t pos = 0, InputControlHandler *handler = nullptr)
        : BaseT(pos, handler), _value(value)
    {
    }

    /** Writes the value as text to the DisplayWriter.
     *  Calls ValueT::ToString() to retrieve that text.
     *  \param output is used to output text and position the cursor.
     */
    void Display(DisplayWriter *output) override
    {
        const char *text = _value->getText();
        if (text != nullptr)
        {
            output->Display(text);
        }
    }

    /** Overridden to increment and decrement the value on the `Up` and `Down` commands.
     *  The control state has to be 'Selected' in order to change the value(s).
     *  \param navCmd is the navigation command.
     *  \return Returns true if the command was handled.
     */
    bool OnNavigationCommand(NavigationCommands navCmd) override
    {
        bool handled = false;

        switch (navCmd)
        {
        case NavigationCommands::Up:
            // LogTrace("Ud:Nav-U");
            handled = TryValueUp();
            break;
        case NavigationCommands::Down:
            // LogTrace("Ud:Nav-D");
            handled = TryValueDown();
            break;
        default:
            break;
        }

        if (handled)
            return true;

        return BaseT::OnNavigationCommand(navCmd);
    }

    /** Attempts to increment the value if the control state is 'Selected'.
     *  \return Returns true if successful.
     */
    bool TryValueUp()
    {
        // LogTrace("Ud:ValUp");
        if (BaseT::getIsSelected())
        {
            _value->IncrementValue();
            return true;
        }

        return false;
    }

    /** Attempts to decrement the value if the control state is 'Selected'.
     *  \return Returns true if successful.
     */
    bool TryValueDown()
    {
        // LogTrace("Ud:ValDwn");
        if (BaseT::getIsSelected())
        {
            _value->DecrementValue();
            return true;
        }

        return false;
    }

private:
    ValueT *_value;
};