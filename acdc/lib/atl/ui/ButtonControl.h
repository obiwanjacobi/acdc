#pragma once
#include "InputControl.h"

enum class ButtonControlEvents
{
    // Different from InputControlEvents::ChangeState
    ChangeState = 0x02
};

/** Represents a single character on the display that can be toggled on or off.
 *  \tparam OffChar the character to show for the off-state.
 *  \tparam OnChar the character to show for the on-state.
 */
template <const uint8_t OffChar, const uint8_t OnChar>
class ButtonControl : public InputControl
{
    typedef InputControl BaseT;

public:
    ButtonControl(uint8_t pos = 0, InputControlHandler *handler = nullptr)
        : BaseT(pos, handler) {}

    bool OnNavigationCommand(NavigationCommands navCmd) override
    {
        // let InputControl manage getting in and out of edit mode (Selected)
        bool handled = BaseT::OnNavigationCommand(navCmd);

        if (!handled)
        {
            switch (navCmd)
            {
            case NavigationCommands::Enter:
                // LogTrace("Btn:Nav-Ent");
                return TryToggleButton();
            default:
                break;
            }
        }

        return handled;
    }

    void Display(DisplayWriter *output) override
    {
        char buffer[2];
        buffer[0] = _buttonState ? OnChar : OffChar;
        buffer[1] = 0;
        output->Display(buffer);
    }

    bool getButtonState() const
    {
        return _buttonState;
    }

    void setButtonState(bool newState)
    {
        _buttonState = newState;
    }

protected:
    bool TryToggleButton()
    {
        if (BaseT::getIsSelected())
        {
            _buttonState = !_buttonState;
            BaseT::HandlerOnInputEvent(Event(ButtonControlEvents::ChangeState), EventParam(_buttonState));
            return true;
        }

        return false;
    }

private:
    bool _buttonState;
};
