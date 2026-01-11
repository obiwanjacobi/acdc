#pragma once
#include <stdint.h>
#include "PanelControlContainer.h"

/** The VerticalPanel implements a Control Panel where controls are lined up vertically.
 *  It overrides the `OnNavigationCommand()` and `Display()` methods to implement its behavior.
 *  \tparam MaxItems is the maximum number of Controls in the Panel.
 */
template <const uint8_t MaxItems>
class VerticalPanel : public PanelControlContainer<MaxItems>
{
    typedef PanelControlContainer<MaxItems> BaseT;

public:
    /** Constructs the instance with an option position.
     *  \param pos is the position relative to its siblings.
     */
    VerticalPanel(uint8_t pos = 0)
        : BaseT(pos)
    {
    }

    /** Adds a Control to the panel. If the Control's position is zero
     *  it is added to the bottom of the panel and it's position is updated.
     *  This makes adding vertically alligned controls easier.
     *  \param control is the control to add. If NULL nothing happens.
     *  \return Returns false when the Panel is full or when control is null.
     */
    bool Add(Control *control)
    {
        if (control == nullptr)
            return false;

        if (BaseT::Add(control))
        {
            if (control->getPosition() == 0)
                control->setPosition((uint8_t)BaseT::getCount() - 1);
            return true;
        }

        return false;
    }

    /** The Panel adjusts the current (selected) control on the `Up` and `Down` commands,
     *  if the BaseT (Panel) did not handle it.
     *  \param navCmd is the navigation command.
     *  \return Returns true when the command was handled, otherwise false.
     */
    bool OnNavigationCommand(NavigationCommands navCmd) override
    {
        bool handled = BaseT::OnNavigationCommand(navCmd);
        if (!handled)
        {
            switch (navCmd)
            {
            case NavigationCommands::Up:
                LogTrace("Vp:Nav-U");
                handled = BaseT::SetPreviousInputControl();
                break;
            case NavigationCommands::Down:
                LogTrace("Vp:Nav-D");
                handled = BaseT::SetNextInputControl();
                break;
            default:
                return false;
            }
        }

        return handled;
    }

    /** Displays all the visible controls in the Panel.
     *  This Display impl does not call the BaseT and all (visible) controls will be displayed at their positions.
     *  \param output is a pointer to the display writer that can be used to output and position text.
     */
    void Display(DisplayWriter *output) override
    {
        for (uint8_t i = 0; i < BaseT::getCount(); i++)
        {
            Control *ctrl = BaseT::GetAt(i);

            if (ctrl->getIsVisible())
            {
                output->GoTo(ctrl->getPosition(), 0);
                ctrl->Display(output);
            }
        }
    }
};