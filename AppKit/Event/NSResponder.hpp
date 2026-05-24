//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/Event/NSResponder.hpp
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

namespace NS
{
class Event;
class Menu;
class MenuItem;
class UndoManager;
class Window;

class Responder : public Referencing<Responder, Object>
{
public:
    Responder* init();
    Responder* init(void* pCoder);

    Responder* nextResponder() const;
    void       setNextResponder(Responder* pResponder);

    bool tryToPerform(SEL action, void* pObject);
    bool performKeyEquivalent(Event* pEvent);

    bool becomeFirstResponder();
    bool resignFirstResponder();
    bool acceptsFirstResponder() const;

    void mouseDown(Event* pEvent);
    void mouseUp(Event* pEvent);
    void keyDown(Event* pEvent);
    void keyUp(Event* pEvent);
    void scrollWheel(Event* pEvent);

    bool validateMenuItem(MenuItem* pMenuItem);

    UndoManager* undoManager() const;
    void         setUndoManager(UndoManager* pUndoManager);

    Window* window() const;
    Menu*   menu() const;
};
}

_AK_INLINE NS::Responder* NS::Responder::init()
{
    return Object::sendMessage<Responder*>(this, _AK_PRIVATE_SEL(init));
}

_AK_INLINE NS::Responder* NS::Responder::init(void* pCoder)
{
    return Object::sendMessage<Responder*>(this, _AK_PRIVATE_SEL(initWithCoder_), pCoder);
}

_AK_INLINE NS::Responder* NS::Responder::nextResponder() const
{
    return Object::sendMessage<Responder*>(this, _AK_PRIVATE_SEL(nextResponder));
}

_AK_INLINE void NS::Responder::setNextResponder(Responder* pResponder)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setNextResponder_), pResponder);
}

_AK_INLINE bool NS::Responder::tryToPerform(SEL action, void* pObject)
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(tryToPerform_with_), action, pObject);
}

_AK_INLINE bool NS::Responder::performKeyEquivalent(Event* pEvent)
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(performKeyEquivalent_), pEvent);
}

_AK_INLINE bool NS::Responder::becomeFirstResponder()
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(becomeFirstResponder));
}

_AK_INLINE bool NS::Responder::resignFirstResponder()
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(resignFirstResponder));
}

_AK_INLINE bool NS::Responder::acceptsFirstResponder() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(acceptsFirstResponder));
}

_AK_INLINE void NS::Responder::mouseDown(Event* pEvent)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(mouseDown_), pEvent);
}

_AK_INLINE void NS::Responder::mouseUp(Event* pEvent)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(mouseUp_), pEvent);
}

_AK_INLINE void NS::Responder::keyDown(Event* pEvent)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(keyDown_), pEvent);
}

_AK_INLINE void NS::Responder::keyUp(Event* pEvent)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(keyUp_), pEvent);
}

_AK_INLINE void NS::Responder::scrollWheel(Event* pEvent)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(scrollWheel_), pEvent);
}

_AK_INLINE bool NS::Responder::validateMenuItem(MenuItem* pMenuItem)
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(validateMenuItem_), pMenuItem);
}

_AK_INLINE NS::UndoManager* NS::Responder::undoManager() const
{
    return Object::sendMessageSafe<UndoManager*>(this, _AK_PRIVATE_SEL(undoManager));
}

_AK_INLINE void NS::Responder::setUndoManager(UndoManager* pUndoManager)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setUndoManager_), pUndoManager);
}

_AK_INLINE NS::Window* NS::Responder::window() const
{
    return Object::sendMessageSafe<Window*>(this, _AK_PRIVATE_SEL(window));
}

_AK_INLINE NS::Menu* NS::Responder::menu() const
{
    return Object::sendMessageSafe<Menu*>(this, _AK_PRIVATE_SEL(menu));
}
