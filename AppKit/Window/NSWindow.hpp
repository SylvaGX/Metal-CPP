//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/Window/NSWindow.hpp
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
#include "Window/NSView.hpp"

namespace NS
{
class Screen;
class String;

class Window : public Responder
{
public:
    Window* initWithContentRect(Rect contentRect, WindowStyleMask style, BackingStoreType backingStoreType, bool defer);

    String* title() const;
    void    setTitle(String* pTitle);

    View* contentView() const;
    void  setContentView(View* pView);

    void makeKeyAndOrderFront(void* pSender);
    void orderOut(void* pSender);
    void close();

    Rect  frame() const;
    void  setFrame(Rect frame, bool display);

    Size  contentSize() const;
    void  setContentSize(Size size);

    Size  minSize() const;
    void  setMinSize(Size size);

    Size  maxSize() const;
    void  setMaxSize(Size size);

    void center();

    WindowLevel level() const;
    void        setLevel(WindowLevel level);

    void toggleFullScreen(void* pSender);
    void miniaturize(void* pSender);
    void deminiaturize(void* pSender);

    bool isMiniaturized() const;
    bool isZoomed() const;
    bool isKeyWindow() const;
    bool isMainWindow() const;

    void makeKeyWindow();
    void makeMainWindow();
    void resignKeyWindow();
    void resignMainWindow();

    void* delegate() const;
    void  setDelegate(void* pDelegate);

    CGFloat backingScaleFactor() const;
    Screen* screen() const;

    void setCollectionBehavior(WindowCollectionBehavior behavior);
};
}

_AK_INLINE NS::Window* NS::Window::initWithContentRect(Rect contentRect, WindowStyleMask style, BackingStoreType backingStoreType, bool defer)
{
    return Object::sendMessage<Window*>(this, _AK_PRIVATE_SEL(initWithContentRect_styleMask_backing_defer_), contentRect, style, backingStoreType, defer);
}

_AK_INLINE NS::String* NS::Window::title() const
{
    return Object::sendMessage<String*>(this, _AK_PRIVATE_SEL(title));
}

_AK_INLINE void NS::Window::setTitle(String* pTitle)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setTitle_), pTitle);
}

_AK_INLINE NS::View* NS::Window::contentView() const
{
    return Object::sendMessage<View*>(this, _AK_PRIVATE_SEL(contentView));
}

_AK_INLINE void NS::Window::setContentView(View* pView)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setContentView_), pView);
}

_AK_INLINE void NS::Window::makeKeyAndOrderFront(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(makeKeyAndOrderFront_), pSender);
}

_AK_INLINE void NS::Window::orderOut(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(orderOut_), pSender);
}

_AK_INLINE void NS::Window::close()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(close));
}

_AK_INLINE NS::Rect NS::Window::frame() const
{
    return Object::sendMessage<Rect>(this, _AK_PRIVATE_SEL(frame));
}

_AK_INLINE void NS::Window::setFrame(Rect frame, bool display)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setFrame_display_), frame, display);
}

_AK_INLINE NS::Size NS::Window::contentSize() const
{
    return Object::sendMessage<Size>(this, _AK_PRIVATE_SEL(contentSize));
}

_AK_INLINE void NS::Window::setContentSize(Size size)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setContentSize_), size);
}

_AK_INLINE NS::Size NS::Window::minSize() const
{
    return Object::sendMessage<Size>(this, _AK_PRIVATE_SEL(minSize));
}

_AK_INLINE void NS::Window::setMinSize(Size size)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setMinSize_), size);
}

_AK_INLINE NS::Size NS::Window::maxSize() const
{
    return Object::sendMessage<Size>(this, _AK_PRIVATE_SEL(maxSize));
}

_AK_INLINE void NS::Window::setMaxSize(Size size)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setMaxSize_), size);
}

_AK_INLINE void NS::Window::center()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(center));
}

_AK_INLINE NS::WindowLevel NS::Window::level() const
{
    return Object::sendMessage<WindowLevel>(this, _AK_PRIVATE_SEL(level));
}

_AK_INLINE void NS::Window::setLevel(WindowLevel level)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setLevel_), level);
}

_AK_INLINE void NS::Window::toggleFullScreen(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(toggleFullScreen_), pSender);
}

_AK_INLINE void NS::Window::miniaturize(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(miniaturize_), pSender);
}

_AK_INLINE void NS::Window::deminiaturize(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(deminiaturize_), pSender);
}

_AK_INLINE bool NS::Window::isMiniaturized() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(isMiniaturized));
}

_AK_INLINE bool NS::Window::isZoomed() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(isZoomed));
}

_AK_INLINE bool NS::Window::isKeyWindow() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(isKeyWindow));
}

_AK_INLINE bool NS::Window::isMainWindow() const
{
    return Object::sendMessage<bool>(this, _AK_PRIVATE_SEL(isMainWindow));
}

_AK_INLINE void NS::Window::makeKeyWindow()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(makeKeyWindow));
}

_AK_INLINE void NS::Window::makeMainWindow()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(makeMainWindow));
}

_AK_INLINE void NS::Window::resignKeyWindow()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(resignKeyWindow));
}

_AK_INLINE void NS::Window::resignMainWindow()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(resignMainWindow));
}

_AK_INLINE void* NS::Window::delegate() const
{
    return Object::sendMessage<void*>(this, _AK_PRIVATE_SEL(delegate));
}

_AK_INLINE void NS::Window::setDelegate(void* pDelegate)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setDelegate_), pDelegate);
}

_AK_INLINE CGFloat NS::Window::backingScaleFactor() const
{
    return Object::sendMessage<CGFloat>(this, _AK_PRIVATE_SEL(backingScaleFactor));
}

_AK_INLINE NS::Screen* NS::Window::screen() const
{
    return Object::sendMessage<Screen*>(this, _AK_PRIVATE_SEL(screen));
}

_AK_INLINE void NS::Window::setCollectionBehavior(WindowCollectionBehavior behavior)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setCollectionBehavior_), behavior);
}
