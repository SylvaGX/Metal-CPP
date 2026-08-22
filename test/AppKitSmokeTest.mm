#define NS_PRIVATE_IMPLEMENTATION
#define AK_PRIVATE_IMPLEMENTATION

#include "AppKit/AppKit.hpp"

static void AppKitSmokeTest()
{
    NS::Window* pWindow = NS::Window::alloc();
    (void)pWindow;

    NS::View* pView = NS::View::alloc();
    pView->setWantsLayer(true);
    pView->setLayer(nullptr);

    NS::Menu* pMenu = NS::Menu::alloc()->initWithTitle(nullptr);
    pMenu->addItemWithTitle(nullptr, nullptr, nullptr, nullptr);
    pMenu->removeAllItems();

    NS::MenuItem* pSeparator = NS::MenuItem::separatorItem();
    (void)pSeparator;

    NS::Application* pApp = NS::Application::sharedApplication();
    (void)pApp->isActive();
    (void)pApp->presentationOptions();

    NS::Event* pEvent = NS::Event::keyEventWithType(
        NS::EventTypeKeyDown,
        NS::Point{0, 0},
        NS::EventModifierFlagCommand,
        0.0,
        0,
        nullptr,
        nullptr,
        nullptr,
        false,
        0);
    (void)pEvent->clickCount();
    (void)pEvent->phase();

    (void)NS::ViewWidthSizable;
    (void)NS::MenuItemStateOn;
    (void)NS::ApplicationPresentationFullScreen;
    (void)NS::DefaultRunLoopMode;
}

int main()
{
    AppKitSmokeTest();
    return 0;
}
