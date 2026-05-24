//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/Window/NSScreen.hpp
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
class Array;
class Dictionary;

class Screen : public Referencing<Screen, Object>
{
public:
    static Array*  screens();
    static Screen* mainScreen();

    Rect    frame() const;
    Rect    visibleFrame() const;
    CGFloat backingScaleFactor() const;
    Dictionary* deviceDescription() const;
};
}

_AK_INLINE NS::Array* NS::Screen::screens()
{
    return Object::sendMessage<Array*>(_AK_PRIVATE_CLS(NSScreen), _AK_PRIVATE_SEL(screens));
}

_AK_INLINE NS::Screen* NS::Screen::mainScreen()
{
    return Object::sendMessage<Screen*>(_AK_PRIVATE_CLS(NSScreen), _AK_PRIVATE_SEL(mainScreen));
}

_AK_INLINE NS::Rect NS::Screen::frame() const
{
    return Object::sendMessage<Rect>(this, _AK_PRIVATE_SEL(frame));
}

_AK_INLINE NS::Rect NS::Screen::visibleFrame() const
{
    return Object::sendMessage<Rect>(this, _AK_PRIVATE_SEL(visibleFrame));
}

_AK_INLINE CGFloat NS::Screen::backingScaleFactor() const
{
    return Object::sendMessage<CGFloat>(this, _AK_PRIVATE_SEL(backingScaleFactor));
}

_AK_INLINE NS::Dictionary* NS::Screen::deviceDescription() const
{
    return Object::sendMessage<Dictionary*>(this, _AK_PRIVATE_SEL(deviceDescription));
}
