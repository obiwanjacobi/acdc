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

/** The InputControl is the abstract base class for all Controls that accept input stimuli.
 *  This class derives from Control and from NavigationController (the input).
 *  It can be 'detected' by the ControlCast using the InputControl enum value.
 */
class InputControl : public Control
{
    typedef Control BaseT;

public:
    InputControl(uint8_t pos = 0) : BaseT(pos) {}

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
        switch (navCmd)
        {
        case NavigationCommands::Enter:
            // LogTrace("Inp:Nav-Ent");
            return TrySelect();
        case NavigationCommands::Exit:
            // LogTrace("Inp:Ext");
            return TryDeselect();
        default:
            break;
        }

        return false;
    }

    /** Attempts to get the Control from the `Focused` to the `Selected` state.
     *  \return Returns true if successful.
     */
    bool TrySelect()
    {
        if (BaseT::getIsFocused())
        {
            BaseT::setState(ControlState::Selected);
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
            BaseT::setState(ControlState::Focused);
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

        // don't allow focus when disabled (or hidden).
        if (newState == ControlState::Focused &&
            !BaseT::getIsEnabled())
        {
            return false;
        }

        return true;
    }
};