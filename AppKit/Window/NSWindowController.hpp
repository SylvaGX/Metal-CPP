//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/Window/NSWindowController.hpp
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
#include "Event/NSResponder.hpp"
#include "Window/NSWindow.hpp"

namespace NS
{
class WindowController : public Responder
{
public:
    static WindowController* alloc();

    WindowController* initWithWindow(Window* pWindow);

    Window* window() const;
    void    setWindow(Window* pWindow);

    void showWindow(void* pSender);
    void close();
};
}

_AK_INLINE NS::WindowController* NS::WindowController::alloc()
{
    return Object::alloc<WindowController>(_AK_PRIVATE_CLS(NSWindowController));
}

_AK_INLINE NS::WindowController* NS::WindowController::initWithWindow(Window* pWindow)
{
    return Object::sendMessage<WindowController*>(this, _AK_PRIVATE_SEL(initWithWindow_), pWindow);
}

_AK_INLINE NS::Window* NS::WindowController::window() const
{
    return Object::sendMessage<Window*>(this, _AK_PRIVATE_SEL(window));
}

_AK_INLINE void NS::WindowController::setWindow(Window* pWindow)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(setWindow_), pWindow);
}

_AK_INLINE void NS::WindowController::showWindow(void* pSender)
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(showWindow_), pSender);
}

_AK_INLINE void NS::WindowController::close()
{
    Object::sendMessage<void>(this, _AK_PRIVATE_SEL(close));
}
