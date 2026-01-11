#pragma once
#include "../lib/atl/ui/DisplayWriter.h"
#include "../lib/atl/ui/Control.h"
#include "../lib/atl/ui/LabelControl.h"
#include "../lib/atl/ui/InputControl.h"
#include "../lib/atl/ui/UpDownControl.h"
#include "../lib/atl/ui/EditControl.h"
#include "../lib/atl/ui/TextControl.h"
#include "../lib/atl/ui/ButtonControl.h"
#include "../lib/atl/ui/Page.h"
#include "../lib/atl/ui/PageController.h"
#include "../lib/atl/ui/Line.h"
#include "../lib/atl/Singleton.h"
#include "HD44780.h"

typedef Page<LcdLines> Screen;

class PageScreen : public Screen
{
public:
    PageScreen()
    {
        Add(&Line1);
        Add(&Line2);
    }

private:
    class PageLine : public Line<3>
    {
        typedef Line<3> BaseT;

    public:
        PageLine()
            : Label("Edit:"), Text(5), Button(15)
        {
            BaseT::Add(&Label);
            BaseT::Add(&Text);
            BaseT::Add(&Button);
        }

        LabelControl Label;
        AsciiTextControl<5> Text;
        ButtonControl<'O', 'X'> Button;
    };

    PageLine Line1;
    PageLine Line2;
};

//-----------------------------------------------------------------------------

#define NumberOfPages 1

class PageManager : public Singleton<PageManager>,
                    public PageController<LcdLines, NumberOfPages>
{
    typedef PageController<LcdLines, NumberOfPages> BaseT;

public:
    typedef Page<LcdLines> PageT;

    PageManager()
    {
        BaseT::Add(&PageScreen1);
        // BaseT::Add(&PageScreen2);
        //  BaseT::Add(&TestScreen);
    }

    PageScreen PageScreen1;
    // PageScreen2 PageScreen2;

    bool Navigate(NavigationCommands navCmd)
    {
        // bypass the next-page logic of PageController
        bool handled = BaseT::OnNavigationCommandCurrentPage(navCmd);

        // Can't remember why I did this exactly: probably to enter the page into focus-mode when enter is pressed.
        // It causes a bug when in edit mode and enter is pressed...
        if (!handled && navCmd == NavigationCommands::Enter)
        {
            LogTrace("PageMgr::Nav Page->Down");
            PageT *page = BaseT::getCurrentPage();
            // if (!page->getIsSelected())
            page->OnNavigationCommand(NavigationCommands::Down);
        }

        return handled;
    }

    void Display(LCD *lcd)
    {
        if (_currentPage != BaseT::getCurrentPage())
        {
            lcd->ClearDisplay();
            lcd->ReturnHome();
            _currentPage = BaseT::getCurrentPage();
        }

        BaseT::Display(lcd);
    }

private:
    PageT *_currentPage;
};