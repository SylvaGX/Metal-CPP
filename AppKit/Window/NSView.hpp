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
class Array;
class Window;

class View : public Responder
{
public:
    static View* alloc();

    View* initWithFrame(Rect frame);

    Rect  frame() const;
    void  setFrame(Rect frame);
    void  setFrameSize(Size size);

    Rect  bounds() const;
    void  setBounds(Rect bounds);

    bool  wantsLayer() const;
    void  setWantsLayer(bool wantsLayer);
    void* layer() const;
    void  setLayer(void* pLayer);

    bool isHidden() const;
    void setHidden(bool hidden);

    View* superview() const;
    Array* subviews() const;

    void addSubview(View* pView);
    void removeFromSuperview();

    Point convertPointToView(Point point, View* pView) const;
    Point convertPointFromView(Point point, View* pView) const;

    void setNeedsDisplay(bool needsDisplay);
    void display();
    void displayIfNeeded();

    View* hitTest(Point point);

    Window* window() const;

    AutoresizingMaskOptions autoresizingMask() const;
    void                    setAutoresizingMask(AutoresizingMaskOptions autoresizingMask);
};
}

_AK_INLINE NS::View* NS::View::alloc()
{
    return Object::alloc<View>(_AK_PRIVATE_CLS(NSView));
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

_AK_INLINE void NS::View::setLayer(void* pLayer)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setLayer_), pLayer);
}

_AK_INLINE bool NS::View::isHidden() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(hidden));
}

_AK_INLINE void NS::View::setHidden(bool hidden)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setHidden_), hidden);
}

_AK_INLINE NS::View* NS::View::superview() const
{
    return Object::sendMessage<View*>(this, _AK_PRIVATE_SEL(superview));
}

_AK_INLINE NS::Array* NS::View::subviews() const
{
    return Object::sendMessage<Array*>(this, _AK_PRIVATE_SEL(subviews));
}

_AK_INLINE void NS::View::addSubview(View* pView)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(addSubview_), pView);
}

_AK_INLINE void NS::View::removeFromSuperview()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(removeFromSuperview));
}

_AK_INLINE NS::Point NS::View::convertPointToView(Point point, View* pView) const
{
    return Object::sendMessage<Point>(this, _AK_PRIVATE_SEL(convertPoint_toView_), point, pView);
}

_AK_INLINE NS::Point NS::View::convertPointFromView(Point point, View* pView) const
{
    return Object::sendMessage<Point>(this, _AK_PRIVATE_SEL(convertPoint_fromView_), point, pView);
}

_AK_INLINE void NS::View::setNeedsDisplay(bool needsDisplay)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setNeedsDisplay_), needsDisplay);
}

_AK_INLINE void NS::View::display()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(display));
}

_AK_INLINE void NS::View::displayIfNeeded()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(displayIfNeeded));
}

_AK_INLINE NS::View* NS::View::hitTest(Point point)
{
    return Object::sendMessage<View*>(this, _AK_PRIVATE_SEL(hitTest_), point);
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
