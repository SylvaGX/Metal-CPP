//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/Window/NSPanel.hpp
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
#include "Window/NSWindow.hpp"

namespace NS
{
class Panel : public Window
{
public:
    static Panel* alloc();

    Panel* initWithContentRect(Rect contentRect, WindowStyleMask style, BackingStoreType backingStoreType, bool defer);

    bool isFloatingPanel() const;
    void setFloatingPanel(bool isFloatingPanel);

    bool worksWhenModal() const;
    void setWorksWhenModal(bool worksWhenModal);
};
}

_AK_INLINE NS::Panel* NS::Panel::alloc()
{
    return Object::alloc<Panel>(_AK_PRIVATE_CLS(NSPanel));
}

_AK_INLINE NS::Panel* NS::Panel::initWithContentRect(Rect contentRect, WindowStyleMask style, BackingStoreType backingStoreType, bool defer)
{
    return Object::sendMessage<Panel*>(this, _AK_PRIVATE_SEL(initWithContentRect_styleMask_backing_defer_), contentRect, style, backingStoreType, defer);
}

_AK_INLINE bool NS::Panel::isFloatingPanel() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(isFloatingPanel));
}

_AK_INLINE void NS::Panel::setFloatingPanel(bool isFloatingPanel)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setFloatingPanel_), isFloatingPanel);
}

_AK_INLINE bool NS::Panel::worksWhenModal() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(worksWhenModal));
}

_AK_INLINE void NS::Panel::setWorksWhenModal(bool worksWhenModal)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setWorksWhenModal_), worksWhenModal);
}
