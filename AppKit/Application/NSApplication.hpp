//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/Application/NSApplication.hpp
//
// Copyright 2026 SylvaGX.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "Foundation.hpp"
#include "AKDefines.hpp"
#include "AKPrivate.hpp"
#include "AKTypes.hpp"
#include "AKAppKitEnums.hpp"
#include "Event/NSResponder.hpp"

namespace NS
{
class Array;
class Date;
class Event;
class Menu;
class String;
class Window;

class Application : public Responder
{
public:
    static Application* sharedApplication();

    Application* init();

    void run();
    void terminate(void* pSender);
    void activate();
    void activateIgnoringOtherApps(bool flag);

    ApplicationActivationPolicy activationPolicy() const;
    void                      setActivationPolicy(ApplicationActivationPolicy activationPolicy);

    void finishLaunching();
    void updateWindows();
    void stop(void* pModalWindow);
    void abortModal();

    Array*  windows() const;
    Window* mainWindow() const;
    Window* keyWindow() const;

    void* delegate() const;
    void  setDelegate(void* pDelegate);

    Menu* mainMenu() const;
    void  setMainMenu(Menu* pMenu);

    Menu* servicesMenu() const;
    void  setServicesMenu(Menu* pMenu);

    Menu* helpMenu() const;
    void  setHelpMenu(Menu* pMenu);

    ApplicationPresentationOptions presentationOptions() const;
    void                           setPresentationOptions(ApplicationPresentationOptions options);

    Event* nextEventMatchingMask(EventMask mask, Date* pExpiration, String* pMode, bool dequeue);
    Event* currentEvent() const;
    void   sendEvent(Event* pEvent);
    void   postEvent(Event* pEvent, bool atStart);
    bool   sendAction(SEL action, void* pTarget, void* pSender);

    bool isRunning() const;
    bool isActive() const;
    bool isHidden() const;

    void hide(void* pSender);
    void unhide(void* pSender);
    void unhideWithoutActivation();
    void hideOtherApplications(void* pSender);
    void unhideAllApplications(void* pSender);
};
}

_AK_INLINE NS::Application* NS::Application::sharedApplication()
{
    return Object::sendMessage<Application*>(_AK_PRIVATE_CLS(NSApplication), _AK_PRIVATE_SEL(sharedApplication));
}

_AK_INLINE NS::Application* NS::Application::init()
{
    return Object::sendMessage<Application*>(this, _AK_PRIVATE_SEL(init));
}

_AK_INLINE void NS::Application::run()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(run));
}

_AK_INLINE void NS::Application::terminate(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(terminate_), pSender);
}

_AK_INLINE void NS::Application::activate()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(activate));
}

_AK_INLINE void NS::Application::activateIgnoringOtherApps(bool flag)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(activateIgnoringOtherApps_), flag);
}

_AK_INLINE NS::ApplicationActivationPolicy NS::Application::activationPolicy() const
{
    return Object::sendMessage<ApplicationActivationPolicy>(this, _AK_PRIVATE_SEL(activationPolicy));
}

_AK_INLINE void NS::Application::setActivationPolicy(ApplicationActivationPolicy activationPolicy)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setActivationPolicy_), activationPolicy);
}

_AK_INLINE void NS::Application::finishLaunching()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(finishLaunching));
}

_AK_INLINE void NS::Application::updateWindows()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(updateWindows));
}

_AK_INLINE void NS::Application::stop(void* pModalWindow)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(stop_), pModalWindow);
}

_AK_INLINE void NS::Application::abortModal()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(abortModal));
}

_AK_INLINE NS::Array* NS::Application::windows() const
{
    return Object::sendMessage<Array*>(this, _AK_PRIVATE_SEL(windows));
}

_AK_INLINE NS::Window* NS::Application::mainWindow() const
{
    return Object::sendMessage<Window*>(this, _AK_PRIVATE_SEL(mainWindow));
}

_AK_INLINE NS::Window* NS::Application::keyWindow() const
{
    return Object::sendMessage<Window*>(this, _AK_PRIVATE_SEL(keyWindow));
}

_AK_INLINE void* NS::Application::delegate() const
{
    return Object::sendMessage<void*>(this, _AK_PRIVATE_SEL(delegate));
}

_AK_INLINE void NS::Application::setDelegate(void* pDelegate)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setDelegate_), pDelegate);
}

_AK_INLINE NS::Menu* NS::Application::mainMenu() const
{
    return Object::sendMessage<Menu*>(this, _AK_PRIVATE_SEL(mainMenu));
}

_AK_INLINE void NS::Application::setMainMenu(Menu* pMenu)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setMainMenu_), pMenu);
}

_AK_INLINE NS::Menu* NS::Application::servicesMenu() const
{
    return Object::sendMessage<Menu*>(this, _AK_PRIVATE_SEL(servicesMenu));
}

_AK_INLINE void NS::Application::setServicesMenu(Menu* pMenu)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setServicesMenu_), pMenu);
}

_AK_INLINE NS::Menu* NS::Application::helpMenu() const
{
    return Object::sendMessage<Menu*>(this, _AK_PRIVATE_SEL(helpMenu));
}

_AK_INLINE void NS::Application::setHelpMenu(Menu* pMenu)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setHelpMenu_), pMenu);
}

_AK_INLINE NS::ApplicationPresentationOptions NS::Application::presentationOptions() const
{
    return Object::sendMessage<ApplicationPresentationOptions>(this, _AK_PRIVATE_SEL(presentationOptions));
}

_AK_INLINE void NS::Application::setPresentationOptions(ApplicationPresentationOptions options)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setPresentationOptions_), options);
}

_AK_INLINE NS::Event* NS::Application::nextEventMatchingMask(EventMask mask, Date* pExpiration, String* pMode, bool dequeue)
{
    return Object::sendMessage<Event*>(this, _AK_PRIVATE_SEL(nextEventMatchingMask_untilDate_inMode_dequeue_), mask, pExpiration, pMode, dequeue);
}

_AK_INLINE NS::Event* NS::Application::currentEvent() const
{
    return Object::sendMessage<Event*>(this, _AK_PRIVATE_SEL(currentEvent));
}

_AK_INLINE void NS::Application::sendEvent(Event* pEvent)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(sendEvent_), pEvent);
}

_AK_INLINE void NS::Application::postEvent(Event* pEvent, bool atStart)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(postEvent_atStart_), pEvent, atStart);
}

_AK_INLINE bool NS::Application::sendAction(SEL action, void* pTarget, void* pSender)
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(sendAction_to_from_), action, pTarget, pSender);
}

_AK_INLINE bool NS::Application::isRunning() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(isRunning));
}

_AK_INLINE bool NS::Application::isActive() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(active));
}

_AK_INLINE bool NS::Application::isHidden() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(hidden));
}

_AK_INLINE void NS::Application::hide(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(hide_), pSender);
}

_AK_INLINE void NS::Application::unhide(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(unhide_), pSender);
}

_AK_INLINE void NS::Application::unhideWithoutActivation()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(unhideWithoutActivation));
}

_AK_INLINE void NS::Application::hideOtherApplications(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(hideOtherApplications_), pSender);
}

_AK_INLINE void NS::Application::unhideAllApplications(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(unhideAllApplications_), pSender);
}
