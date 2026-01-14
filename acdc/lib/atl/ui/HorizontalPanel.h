#pragma once
#include <stdint.h>
#include "PanelControlContainer.h"

/** The HorizontalPanel implements a Control Panel where controls are lined up horizontally.
 *  It overrides the `OnNavigationCommand()` and `Display()` methods to implement its behavior.
 *  \tparam MaxItems is the maximum number of Controls in the Panel.
 */
template <const uint8_t MaxItems>
class HorizontalPanel : public PanelControlContainer<MaxItems>
{
public:
    typedef PanelControlContainer<MaxItems> BaseT;

    /** Constructs the instance with an option position.
     *  \param pos is the position relative to its siblings.
     */
    HorizontalPanel(uint8_t pos = 0)
        : BaseT(pos) {}

    /** Routes the command to its BaseT and implements Panel navigation.
     *  If the navCmd is not handled by BaseT the Panel adjusts the current (selected) control
     *  on the `Left` and `Right` commands.
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
            case NavigationCommands::Left:
                // LogTrace("Hp:Nav-L");
                handled = BaseT::SetPreviousInputControl();
                break;
            case NavigationCommands::Right:
                // LogTrace("Hp:Nav-R");
                handled = BaseT::SetNextInputControl();
                break;
            default:
                break;
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
                output->GoTo(DisplayWriter::CurrentPos, ctrl->getPosition());
                ctrl->Display(output);
            }
        }
    }
};