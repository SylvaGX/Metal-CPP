//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/Menu/NSMenu.hpp
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
#include "Menu/NSMenuItem.hpp"

namespace NS
{
class String;
class View;

class Menu : public Referencing<Menu, Object>
{
public:
    static Menu* alloc();

    Menu* init();
    Menu* initWithTitle(String* pTitle);

    void addItem(MenuItem* pItem);
    MenuItem* addItemWithTitle(String* pTitle, SEL action, void* pTarget, String* pKeyEquivalent);
    void insertItem(MenuItem* pItem, Integer index);
    void removeItem(MenuItem* pItem);
    void removeItemAtIndex(Integer index);
    void removeAllItems();

    MenuItem* itemAtIndex(Integer index) const;
    Integer   numberOfItems() const;
    MenuItem* itemWithTitle(String* pTitle) const;
    MenuItem* itemWithTag(Integer tag) const;

    String* title() const;
    void    setTitle(String* pTitle);

    void popUpMenuPositioningItem(MenuItem* pItem, Point location, View* pView);
    void performActionForItemAtIndex(Integer index);

    bool autoenablesItems() const;
    void setAutoenablesItems(bool autoenablesItems);

    void* delegate() const;
    void  setDelegate(void* pDelegate);
};
}

_AK_INLINE NS::Menu* NS::Menu::alloc()
{
    return Object::alloc<Menu>(_AK_PRIVATE_CLS(NSMenu));
}

_AK_INLINE NS::Menu* NS::Menu::init()
{
    return Object::sendMessage<Menu*>(this, _AK_PRIVATE_SEL(init));
}

_AK_INLINE NS::Menu* NS::Menu::initWithTitle(String* pTitle)
{
    return Object::sendMessage<Menu*>(this, _AK_PRIVATE_SEL(initWithTitle_), pTitle);
}

_AK_INLINE void NS::Menu::addItem(MenuItem* pItem)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(addItem_), pItem);
}

_AK_INLINE NS::MenuItem* NS::Menu::addItemWithTitle(String* pTitle, SEL action, void* pTarget, String* pKeyEquivalent)
{
    return Object::sendMessage<MenuItem*>(this, _AK_PRIVATE_SEL(addItemWithTitle_action_keyEquivalent_), pTitle, action, pTarget, pKeyEquivalent);
}

_AK_INLINE void NS::Menu::insertItem(MenuItem* pItem, Integer index)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(insertItem_atIndex_), pItem, index);
}

_AK_INLINE void NS::Menu::removeItem(MenuItem* pItem)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(removeItem_), pItem);
}

_AK_INLINE void NS::Menu::removeItemAtIndex(Integer index)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(removeItemAtIndex_), index);
}

_AK_INLINE void NS::Menu::removeAllItems()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(removeAllItems));
}

_AK_INLINE NS::MenuItem* NS::Menu::itemAtIndex(Integer index) const
{
    return Object::sendMessage<MenuItem*>(this, _AK_PRIVATE_SEL(itemAtIndex_), index);
}

_AK_INLINE NS::Integer NS::Menu::numberOfItems() const
{
    return Object::sendMessage<Integer>(this, _AK_PRIVATE_SEL(numberOfItems));
}

_AK_INLINE NS::MenuItem* NS::Menu::itemWithTitle(String* pTitle) const
{
    return Object::sendMessage<MenuItem*>(this, _AK_PRIVATE_SEL(itemWithTitle_), pTitle);
}

_AK_INLINE NS::MenuItem* NS::Menu::itemWithTag(Integer tag) const
{
    return Object::sendMessage<MenuItem*>(this, _AK_PRIVATE_SEL(itemWithTag_), tag);
}

_AK_INLINE NS::String* NS::Menu::title() const
{
    return Object::sendMessage<String*>(this, _AK_PRIVATE_SEL(title));
}

_AK_INLINE void NS::Menu::setTitle(String* pTitle)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setTitle_), pTitle);
}

_AK_INLINE void NS::Menu::popUpMenuPositioningItem(MenuItem* pItem, Point location, View* pView)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(popUpMenuPositioningItem_atLocation_inView_), pItem, location, pView);
}

_AK_INLINE void NS::Menu::performActionForItemAtIndex(Integer index)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(performActionForItemAtIndex_), index);
}

_AK_INLINE bool NS::Menu::autoenablesItems() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(autoenablesItems));
}

_AK_INLINE void NS::Menu::setAutoenablesItems(bool autoenablesItems)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setAutoenablesItems_), autoenablesItems);
}

_AK_INLINE void* NS::Menu::delegate() const
{
    return Object::sendMessage<void*>(this, _AK_PRIVATE_SEL(delegate));
}

_AK_INLINE void NS::Menu::setDelegate(void* pDelegate)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setDelegate_), pDelegate);
}
