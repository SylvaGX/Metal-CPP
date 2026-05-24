//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/Menu/NSMenuItem.hpp
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

namespace NS
{
class Menu;
class String;

class MenuItem : public Referencing<MenuItem, Object>
{
public:
    MenuItem* init();
    MenuItem* initWithTitle(String* pTitle, SEL action, void* pTarget, String* pKeyEquivalent);

    String* title() const;
    void    setTitle(String* pTitle);

    SEL   action() const;
    void  setAction(SEL action);

    void* target() const;
    void  setTarget(void* pTarget);

    String* keyEquivalent() const;
    void    setKeyEquivalent(String* pKeyEquivalent);

    EventModifierFlags keyEquivalentModifierMask() const;
    void               setKeyEquivalentModifierMask(EventModifierFlags mask);

    Menu* submenu() const;
    void  setSubmenu(Menu* pMenu);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    Integer tag() const;
    void    setTag(Integer tag);

    Integer state() const;
    void    setState(Integer state);
};
}

_AK_INLINE NS::MenuItem* NS::MenuItem::init()
{
    return Object::sendMessage<MenuItem*>(this, _AK_PRIVATE_SEL(init));
}

_AK_INLINE NS::MenuItem* NS::MenuItem::initWithTitle(String* pTitle, SEL action, void* pTarget, String* pKeyEquivalent)
{
    return Object::sendMessage<MenuItem*>(this, _AK_PRIVATE_SEL(initWithTitle_action_keyEquivalent_), pTitle, action, pTarget, pKeyEquivalent);
}

_AK_INLINE NS::String* NS::MenuItem::title() const
{
    return Object::sendMessage<String*>(this, _AK_PRIVATE_SEL(title));
}

_AK_INLINE void NS::MenuItem::setTitle(String* pTitle)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setTitle_), pTitle);
}

_AK_INLINE SEL NS::MenuItem::action() const
{
    return Object::sendMessage<SEL>(this, _AK_PRIVATE_SEL(action));
}

_AK_INLINE void NS::MenuItem::setAction(SEL action)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setAction_), action);
}

_AK_INLINE void* NS::MenuItem::target() const
{
    return Object::sendMessage<void*>(this, _AK_PRIVATE_SEL(target));
}

_AK_INLINE void NS::MenuItem::setTarget(void* pTarget)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setTarget_), pTarget);
}

_AK_INLINE NS::String* NS::MenuItem::keyEquivalent() const
{
    return Object::sendMessage<String*>(this, _AK_PRIVATE_SEL(keyEquivalent));
}

_AK_INLINE void NS::MenuItem::setKeyEquivalent(String* pKeyEquivalent)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setKeyEquivalent_), pKeyEquivalent);
}

_AK_INLINE NS::EventModifierFlags NS::MenuItem::keyEquivalentModifierMask() const
{
    return Object::sendMessage<EventModifierFlags>(this, _AK_PRIVATE_SEL(keyEquivalentModifierMask));
}

_AK_INLINE void NS::MenuItem::setKeyEquivalentModifierMask(EventModifierFlags mask)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setKeyEquivalentModifierMask_), mask);
}

_AK_INLINE NS::Menu* NS::MenuItem::submenu() const
{
    return Object::sendMessage<Menu*>(this, _AK_PRIVATE_SEL(submenu));
}

_AK_INLINE void NS::MenuItem::setSubmenu(Menu* pMenu)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setSubmenu_), pMenu);
}

_AK_INLINE bool NS::MenuItem::isEnabled() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(isEnabled));
}

_AK_INLINE void NS::MenuItem::setEnabled(bool enabled)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setEnabled_), enabled);
}

_AK_INLINE NS::Integer NS::MenuItem::tag() const
{
    return Object::sendMessage<Integer>(this, _AK_PRIVATE_SEL(tag));
}

_AK_INLINE void NS::MenuItem::setTag(Integer tag)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setTag_), tag);
}

_AK_INLINE NS::Integer NS::MenuItem::state() const
{
    return Object::sendMessage<Integer>(this, _AK_PRIVATE_SEL(state));
}

_AK_INLINE void NS::MenuItem::setState(Integer state)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setState_), state);
}
