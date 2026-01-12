#pragma once
#include "../lib/atl/ui/DisplayWriter.h"
#include "../lib/atl/ui/Control.h"
#include "../lib/atl/ui/LabelControl.h"
#include "../lib/atl/ui/InputControl.h"
#include "../lib/atl/ui/UpDownControl.h"
#include "../lib/atl/ui/EditControl.h"
#include "../lib/atl/ui/TextControl.h"
#include "../lib/atl/ui/ButtonControl.h"
#include "../lib/atl/ui/OptionsControl.h"
#include "../lib/atl/ui/Page.h"
#include "../lib/atl/ui/PageController.h"
#include "../lib/atl/ui/Line.h"
#include "../lib/atl/FixedArray.h"
#include "../lib/atl/Singleton.h"
#include "HD44780.h"

const char *math_sin = "sin";
const char *math_cos = "cos";
const char *math_tan = "tan";

typedef Page<LcdLines> Screen;

class TraceHandler : public InputControlHandler
{
public:
    bool OnNavigationCommand(InputControl *control, NavigationCommands navCmd) override
    {
        LogTrace("Hnd:Nav");
        return false;
    }
    void OnInputEvent(InputControl *control, uint8_t event, uint16_t param, void *ptr) override
    {
        LogTrace("Hnd:Evt");
    }
};

class PageScreen : public Screen
{
public:
    PageScreen()
    {
        Add(&Line1);
        Add(&Line2);
    }

private:
    class PageLine : public Line<4>
    {
        typedef Line<4> BaseT;

    public:
        PageLine()
            : Label("Edit:"), Text(5, &_traceHandler), Options(&_options, 11), Button(15, &_traceHandler)
        {
            _options.SetAt(0, math_sin);
            _options.SetAt(1, math_cos);
            _options.SetAt(2, math_tan);

            BaseT::Add(&Label);
            BaseT::Add(&Text);
            BaseT::Add(&Options);
            BaseT::Add(&Button);
        }

        LabelControl Label;
        AsciiTextControl<5> Text;
        OptionsControl<FixedArray<const char *, 3>> Options;
        ButtonControl<'O', 'X'> Button;

    private:
        FixedArray<const char *, 3> _options;
    };

private:
    PageLine Line1;
    PageLine Line2;

    static TraceHandler _traceHandler;
};

TraceHandler PageScreen::_traceHandler;

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
    }

    PageScreen PageScreen1;
    // PageScreen2 PageScreen2;

    bool OnNavigationCommand(NavigationCommands navCmd) override
    {
        // bypass the next-page logic of PageController
        bool handled = BaseT::OnNavigationCommandCurrentPage(navCmd);

        // Enter the page into focus-mode when enter is pressed.
        if (!handled && navCmd == NavigationCommands::Enter)
        {
            // LogTrace("PgMgr:Nav Pg->Dwn");
            PageT *page = BaseT::getCurrentPage();
            if (!page->getIsActive())
                handled = page->OnNavigationCommand(NavigationCommands::Down);
        }

        return handled;
    }

    void Display(LCD *lcd)
    {
        PageT *curPage = BaseT::getCurrentPage();
        if (_currentPage != curPage)
        {
            lcd->ClearDisplay();
            lcd->ReturnHome();
            _currentPage = curPage;
        }

        BaseT::Display(lcd);
    }

private:
    PageT *_currentPage;
};