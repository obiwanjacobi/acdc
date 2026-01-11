#pragma once
#include <stdint.h>
#include "../EnumClass.h"

enum class ControlTypes
{
    /** Control type is not set or unknown. */
    Unknown = 0,
    /** Control type is the root class for controls. */
    Control = 0x01,
    /** Control type is an input control (navigation). */
    InputControl = 0x02,
    /** Control type is a control panel (current selection). */
    Panel = 0x03,
    /** Control type is page (lines with controls). */
    Page = 0x04,
};

/** Only one state at a time is active for a control */
enum class ControlState
{
    /** Control state is normal - no special considerations. */
    Normal, // no other states active
    /** Control state is hidden, it will not be displayed and cannot receive input. */
    Hidden, // control is not displayed
    /** Control state is disabled, it cannot receive input but it is still displayed. */
    Disabled, // control is displayed as read-only
    /** Control state is focused, the cursor is displayed at its start.
     *  Only one Control can have the focus at one time. */
    Focused, // control is high-lighted
    /** Control state is selected, the edit-cursor is displayed at its start.
     *  Only one Control can have be selected at one time. */
    Selected, // control is active/selected (entered)
};

/** The Control class is the root base class for all controls in the UI framework.
 *  It implements that basic interface for interacting with all controls.
 */
class Control
{
public:
    Control(uint8_t pos = 0, ControlState state = ControlState::Normal)
        : _pos(pos), _state(state)
    {
    }

    uint8_t getPosition() const { return _pos; }
    void setPosition(uint8_t newPos) { _pos = newPos; }

    bool getIsEnabled() const { return getIsVisible() && !getIsDisabled(); }
    bool getIsActive() const { return getIsFocused() || getIsSelected(); }
    bool getIsVisible() const { return _state != ControlState::Hidden; }
    bool getIsDisabled() const { return _state == ControlState::Disabled; }
    bool getIsFocused() const { return _state == ControlState::Focused; }
    bool getIsSelected() const { return _state == ControlState::Selected; }
    bool setState(ControlState newState)
    {
        if (!BeforeChangeState(newState))
            return false;

        _state = newState;
        return true;
    }

    /** Called to let the Control draw itself using the DisplayWriter.
     *  At Control level this method does nothing.
     *  \tparam DisplayWriterT a DisplayWriter impl.
     *  \param output is a pointer to the DisplayWriter object.
     */
    virtual void Display(DisplayWriter * /*output*/)
    {
    }
    /** Called to let the Control draw its cursor using the DisplayWriter.
     *  At Control level this method does nothing.
     *  \tparam DisplayWriterT a DisplayWriter impl.
     *  \param output is a pointer to the DisplayWriter object.
     */
    virtual void DisplayCursor(DisplayWriter * /*output*/)
    {
    }

    /** Indicates if this object is of the specified type.
     *  \param type indicates the requested type.
     *  \return Returns true when the object is of the same type.
     */
    virtual bool IsOfType(ControlTypes type) const
    {
        return HasFlag(type, ControlTypes::Control);
    }

protected:
    /** Called by `setState()` to allow derived classes to prevent state changes.
     *  This impl will not allow setting the same state multiple times.
     *  \param newState is the proposed state to set. The current state can be retrieved with `getState()`.
     *  \return Returns true if the state change is allowed.
     */
    virtual bool BeforeChangeState(ControlState newState)
    {
        return _state != newState;
    }

private:
    uint8_t _pos;
    ControlState _state;
};

/** Dynamically casts a Control(Base) pointer to the specified type.
 *  \param ctrl is the pointer to the Control. Can be NULL.
 *  \param type is the Control type to cast ctrl to.
 *  \return Returns NULL if ctrl is NULL or ctrl is not of the same type. Otherwise the ctrl pointer is returned.
 */
static Control *ControlCast(Control *ctrl, ControlTypes type)
{
    if (ctrl == nullptr)
        return nullptr;
    if (ctrl->IsOfType(type))
        return ctrl;
    return nullptr;
}
