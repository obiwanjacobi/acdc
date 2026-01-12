#pragma once
#include "DisplayWriter.h"
#include "InputControl.h"

/** The Panel abstract class maintains a reference to the current control.
 */
class Panel : public InputControl
{
    typedef InputControl BaseT;

public:
    Panel(uint8_t pos = 0, InputControlHandler *handler = nullptr)
        : BaseT(pos, handler) {}

    /** Retrieves the current control.
     *  \return Returns NULL if no current control is set.
     */
    InputControl *getCurrentControl() const
    {
        return _currentControl;
    }

    /** Sets the current control to ctrl.
     *  The old current control (if any) will be set to `Normal`.
     *  The new current control (ctrl - if not NULL) will be set to `Focused`.
     *  The state of the Panel will reflect the state of the current control.
     *  \param ctrl is the control make current. Can be NULL.
     */
    void setCurrentControl(InputControl *ctrl)
    {
        setCurrentControlState(ControlState::Normal);

        _currentControl = ctrl;

        setCurrentControlState(ControlState::Focused);
    }

    /** Forwards the call unchanged to the current control (if any).
     *  \param output is used to output text and set the cursor position.
     */
    void Display(DisplayWriter *output) override
    {
        if (_currentControl != nullptr)
            _currentControl->Display(output);
    }
    /** Forwards the call unchanged to the current control (if any).
     *  \param output is used to output text and set the cursor position.
     */
    void DisplayCursor(DisplayWriter *output) override
    {
        if (_currentControl != nullptr)
            _currentControl->DisplayCursor(output);
    }

    /** Forwards the call unchanged to the current control (if any).
     *  \param navCmd is the navigation command.
     */
    bool OnNavigationCommand(NavigationCommands navCmd) override
    {
        // LogTrace("Pnl:Nav");
        if (_currentControl != nullptr)
        {
            // StringWriter<20> writer;
            // writer.Write("Pnl:Nav-curCtrl ");
            // writer.Write((uint8_t)navCmd);
            // LogTrace(writer);
            return _currentControl->OnNavigationCommand(navCmd);
        }
        return false;
    }

    /** Overridden to respond to the `Panel` type.
     *  \param type the requested type.
     *  \return Returns true if the type is part of the class hierarchy.
     */
    bool IsOfType(ControlTypes type) const override
    {
        return HasFlag(type, ControlTypes::Panel) || BaseT::IsOfType(type);
    }

protected:
    /** For derived classes.
     *  \param pos is the optional control position.
     */
    Panel(uint8_t pos = 0) : InputControl(pos), _currentControl(nullptr) {}

    /** Overridden to disallow `Selected` - you cannot edit a Panel.
     *  \param newState is the proposed state.
     *  \return Returns true if the state change may occur.
     */
    bool BeforeChangeState(ControlState newState) override
    {
        bool success = BaseT::BeforeChangeState(newState) &&
                       newState != ControlState::Selected;

        if (success)
        {
            // If the panel is un-'focused', remove the current control.
            // Then if the panel is 'focused' again, the first control will be made current.
            if (newState == ControlState::Normal)
                setCurrentControl(nullptr);
        }

        return success;
    }

private:
    InputControl *_currentControl;

    void setCurrentControlState(ControlState newState)
    {
        if (_currentControl != nullptr &&
            _currentControl->setState(newState))
        {
            // Panel takes on the state of the current control.
            setState(newState);
        }
    }
};