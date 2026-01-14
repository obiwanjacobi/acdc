#pragma once
#include <stdint.h>
#include "PanelControlContainer.h"

/** The PageController class manages multiple pages and the transition of one page to another.
 *  \tparam LinesPerPage is the number of lines per page (must be the same for all pages).
 *  \tparam MaxPages is the maximum number of pages.
 */
template <const uint8_t LinesPerPage, const uint8_t MaxPages>
class PageController : public PanelControlContainer<MaxPages>
{
public:
    typedef PanelControlContainer<MaxPages> BaseT;

    /** The Page type. */
    typedef Page<LinesPerPage> PageT;

    /** Retrieves the current active page.
     *  \return Returns NULL when no active page is set.
     */
    PageT *getCurrentPage() const
    {
        return (PageT *)BaseT::getCurrentControl();
    }

    /** Navigates to a new page when `Up` or `Down` remain unhandled.
     *  \param navCmd is the navigation command.
     *  \return Returns false when the navCmd was not handled.
     */
    bool OnNavigationCommand(NavigationCommands navCmd) override
    {
        bool handled = BaseT::OnNavigationCommand(navCmd);
        if (!handled)
        {
            switch (navCmd)
            {
            case NavigationCommands::Up:
                // LogTrace("PgCt:Nav-Up");
                handled = TrySetPreviousPage();
                break;
            case NavigationCommands::Down:
                // LogTrace("PgCt:Nav-Down");
                handled = TrySetNextPage();
                break;
            default:
                break;
            }
        }

        return handled;
    }

    /** Attempts to set the first page.
     *  \return Returns true when successful.
     */
    bool TrySetFirstPage()
    {
        return BaseT::SetFirstInputControl();
    }

    /** Attempts to set the next page.
     *  \return Returns true when successful.
     */
    bool TrySetNextPage()
    {
        return BaseT::SetNextInputControl();
    }

    /** Attempts to set the previous page.
     *  \return Returns true when successful.
     */
    bool TrySetPreviousPage()
    {
        return BaseT::SetPreviousInputControl();
    }

protected:
    /** Navigates to the lines and controls of the current page.
     *  \param navCmd is the navigation command.
     *  \return Returns false when the navCmd was not handled.
     */
    bool OnNavigationCommandCurrentPage(NavigationCommands navCmd)
    {
        // LogTrace("PageCtrlr:OnNavCurPage");
        return BaseT::OnNavigationCommand(navCmd);
    }
};