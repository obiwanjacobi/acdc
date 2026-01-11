#pragma once
#include "Control.h"

/** The LabelControl displays a static text and does not accept any input.
 *  The LabelControl is not an InputControl so it does not accept navigation commands.
 */
class LabelControl : public Control
{
    typedef Control BaseT;

public:
    /** Constructs the instance with an optional position.
     *  Call `setText()` to assign text to the control.
     *  \param pos is the position relative to its siblings.
     */
    LabelControl(uint8_t pos = 0)
        : Control(pos, ControlState::Disabled), _text(nullptr)
    {
    }

    /** Constructs an initialized instance with an optional position.
     *  \param text points to a zero-terminated string. The string is NOT copied.
     *  \param pos is the position relative to its siblings.
     */
    LabelControl(const char *text, uint8_t pos = 0)
        : Control(pos, ControlState::Disabled), _text(text)
    {
    }

    /** Outputs the text to the display (if available).
     *  Does nothing in modeCursor.
     *  \param output is a pointer to the display writer that is used to output and position text.
     */
    void Display(DisplayWriter *output) override
    {
        if (_text != nullptr)
        {
            output->Display(_text);
        }
    }

    /** Retrieves the text the LabelControl displays.
     *  \return Returns the pointer to the text.
     */
    const char *getText() const
    {
        return _text;
    }

    /** Assigns the text the LabelControl displays.
     *  \param text points to a zero-terminated string. The string is NOT copied.
     */
    void setText(const char *text)
    {
        _text = text;
    }

protected:
    /** Overridden to disallow `Focused` and `Selected` states.
     *  \param newState is the proposed state.
     */
    bool BeforeChangeState(ControlState newState) override
    {
        return BaseT::BeforeChangeState(newState) &&
               newState != ControlState::Focused &&
               newState != ControlState::Selected;
    }

private:
    const char *_text;
};