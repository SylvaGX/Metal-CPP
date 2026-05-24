//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/Window/NSView.hpp
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
class Window;

class View : public Responder
{
public:
    View* initWithFrame(Rect frame);

    Rect  frame() const;
    void  setFrame(Rect frame);
    void  setFrameSize(Size size);

    Rect  bounds() const;
    void  setBounds(Rect bounds);

    bool  wantsLayer() const;
    void  setWantsLayer(bool wantsLayer);
    void* layer() const;

    void addSubview(View* pView);
    void removeFromSuperview();

    Window* window() const;

    AutoresizingMaskOptions autoresizingMask() const;
    void                    setAutoresizingMask(AutoresizingMaskOptions autoresizingMask);
};
}

_AK_INLINE NS::View* NS::View::initWithFrame(Rect frame)
{
    return Object::sendMessage<View*>(this, _AK_PRIVATE_SEL(initWithFrame_), frame);
}

_AK_INLINE NS::Rect NS::View::frame() const
{
    return Object::sendMessage<Rect>(this, _AK_PRIVATE_SEL(frame));
}

_AK_INLINE void NS::View::setFrame(Rect frame)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setFrame_), frame);
}

_AK_INLINE void NS::View::setFrameSize(Size size)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setFrameSize_), size);
}

_AK_INLINE NS::Rect NS::View::bounds() const
{
    return Object::sendMessage<Rect>(this, _AK_PRIVATE_SEL(bounds));
}

_AK_INLINE void NS::View::setBounds(Rect bounds)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setBounds_), bounds);
}

_AK_INLINE bool NS::View::wantsLayer() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(wantsLayer));
}

_AK_INLINE void NS::View::setWantsLayer(bool wantsLayer)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setWantsLayer_), wantsLayer);
}

_AK_INLINE void* NS::View::layer() const
{
    return Object::sendMessage<void*>(this, _AK_PRIVATE_SEL(layer));
}

_AK_INLINE void NS::View::addSubview(View* pView)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(addSubview_), pView);
}

_AK_INLINE void NS::View::removeFromSuperview()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(removeFromSuperview));
}

_AK_INLINE NS::Window* NS::View::window() const
{
    return Object::sendMessage<Window*>(this, _AK_PRIVATE_SEL(window));
}

_AK_INLINE NS::AutoresizingMaskOptions NS::View::autoresizingMask() const
{
    return Object::sendMessage<AutoresizingMaskOptions>(this, _AK_PRIVATE_SEL(autoresizingMask));
}

_AK_INLINE void NS::View::setAutoresizingMask(AutoresizingMaskOptions autoresizingMask)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setAutoresizingMask_), autoresizingMask);
}
