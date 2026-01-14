#pragma once
#include <stdint.h>
#include "Control.h"
#include "../EnumClass.h"

enum class NavigationCommands
{
    /** Not a navigation command. */
    None,
    /** Navigate Up. */
    Up,
    /** Navigate Down. */
    Down,
    /** Navigate Left. */
    Left,
    /** Navigate Right. */
    Right,
    /** Enter edit mode. */
    Enter,
    /** Exit edit mode. */
    Exit
};

enum class InputControlEvents
{
    /** param=ControlState ptr=nullptr */
    ChangeState = 0x01,
};

#define Event(e) (uint8_t)e
#define EventParam(p) (uint16_t)p
#define EventPtr(p) (void *)p

class InputControl; // fwd decl
class InputControlHandler
{
public:
    // called to allow to process a navigation command before InputControl's logic is executed.
    virtual bool OnNavigationCommand(InputControl *control, NavigationCommands navCmd) { return false; }
    // called to notify of a control-specific event.
    virtual void OnInputEvent(InputControl *control, uint8_t event, uint16_t param, void *ptr) {}
};

/** The InputControl is the abstract base class for all Controls that accept input stimuli.
 *  This class derives from Control and from NavigationController (the input).
 *  It can be 'detected' by the ControlCast using the InputControl enum value.
 */
class InputControl : public Control
{
public:
    typedef Control BaseT;

    InputControl(uint8_t pos = 0, InputControlHandler *handler = nullptr)
        : BaseT(pos), _handler(handler) {}

    /** Implements selection behavior on the `Enter` and `Exit` navigation commands.
     *  If the Control is in `Focused` and the `Enter` commands is provided, it will
     *  try to transition to the `Selected` state (`TrySelect()`). If the Control is
     *  `Selected` and the `Exit` command is provided, it will try to transition the
     *  `Focused` state (`TryDeselect()`).
     *  \param navCmd the navigation command.
     *  \return Returns true when the command was handled.
     */
    virtual bool OnNavigationCommand(NavigationCommands navCmd)
    {
        bool handled = HandlerOnNavigationCommand(navCmd);

        if (!handled)
        {
            switch (navCmd)
            {
            case NavigationCommands::Enter:
                // LogTrace("Inp:Nav-Ent");
                handled = TrySelect();
                break;
            case NavigationCommands::Exit:
                // LogTrace("Inp:Ext");
                handled = TryDeselect();
                break;
            default:
                break;
            }
        }

        return handled;
    }

    /** Attempts to get the Control from the `Focused` to the `Selected` state.
     *  \return Returns true if successful.
     */
    bool TrySelect()
    {
        if (BaseT::getIsFocused())
        {
            if (BaseT::setState(ControlState::Selected))
                HandlerOnInputEvent(Event(InputControlEvents::ChangeState), EventParam(ControlState::Selected));

            return true;
        }
        return false;
    }

    /** Attempts to get the Control from the `Selected` to the `Focused` state.
     *  \return Returns true if successful.
     */
    bool TryDeselect()
    {
        if (BaseT::getIsSelected())
        {
            if (BaseT::setState(ControlState::Focused))
                HandlerOnInputEvent(Event(InputControlEvents::ChangeState), EventParam(ControlState::Focused));

            return true;
        }
        return false;
    }

    /** Overridden to implement the InputControl type flag.
     *  \param type indicates the requested type.
     *  \return Returns true if the type flag matches the class hierarchy.
     */
    bool IsOfType(ControlTypes type) const override
    {
        return HasFlag(type, ControlTypes::InputControl) ||
               BaseT::IsOfType(type);
    }

protected:
    /** Overridden to disallow going to the `Focused` when not enabled.
     *  \param newState is the proposed state.
     */
    bool BeforeChangeState(ControlState newState) override
    {
        if (!BaseT::BeforeChangeState(newState))
            return false;

        // don't allow focus/select when disabled (or hidden).
        if ((newState == ControlState::Focused || newState == ControlState::Selected) &&
            !BaseT::getIsEnabled())
        {
            return false;
        }

        HandlerOnInputEvent(Event(InputControlEvents::ChangeState), EventParam(newState));
        return true;
    }

    bool HandlerOnNavigationCommand(NavigationCommands navCmd)
    {
        if (_handler == nullptr)
            return false;

        return _handler->OnNavigationCommand(this, navCmd);
    }

    bool HandlerOnInputEvent(uint8_t event, uint16_t param, void *ptr = nullptr)
    {
        if (_handler == nullptr)
            return false;

        _handler->OnInputEvent(this, event, param, ptr);
        return true;
    }

private:
    InputControlHandler *_handler;
};