//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/Event/NSEvent.hpp
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
class Window;

class Event : public Referencing<Event, Object>
{
public:
    static Event* mouseEventWithType(EventType type, Point location, EventModifierFlags flags, TimeInterval timestamp, Integer windowNumber, void* pContext, Integer eventNumber, Integer clickCount, float pressure);

    EventType type() const;
    Window*   window() const;
    Point     locationInWindow() const;
    UInteger  modifierFlags() const;
    String*   characters() const;
    String*   charactersIgnoringModifiers() const;
    Integer   keyCode() const;
    CGFloat   deltaX() const;
    CGFloat   deltaY() const;
    TimeInterval timestamp() const;
    Integer   windowNumber() const;
};
}

_AK_INLINE NS::Event* NS::Event::mouseEventWithType(EventType type, Point location, EventModifierFlags flags, TimeInterval timestamp, Integer windowNumber, void* pContext, Integer eventNumber, Integer clickCount, float pressure)
{
    return Object::sendMessage<Event*>(_AK_PRIVATE_CLS(NSEvent), _AK_PRIVATE_SEL(mouseEventWithType_location_modifierFlags_timestamp_windowNumber_context_eventNumber_clickCount_pressure_), type, location, flags, timestamp, windowNumber, pContext, eventNumber, clickCount, pressure);
}

_AK_INLINE NS::EventType NS::Event::type() const
{
    return Object::sendMessage<EventType>(this, _AK_PRIVATE_SEL(type));
}

_AK_INLINE NS::Window* NS::Event::window() const
{
    return Object::sendMessage<Window*>(this, _AK_PRIVATE_SEL(window));
}

_AK_INLINE NS::Point NS::Event::locationInWindow() const
{
    return Object::sendMessage<Point>(this, _AK_PRIVATE_SEL(locationInWindow));
}

_AK_INLINE NS::UInteger NS::Event::modifierFlags() const
{
    return Object::sendMessage<UInteger>(this, _AK_PRIVATE_SEL(modifierFlags));
}

_AK_INLINE NS::String* NS::Event::characters() const
{
    return Object::sendMessage<String*>(this, _AK_PRIVATE_SEL(characters));
}

_AK_INLINE NS::String* NS::Event::charactersIgnoringModifiers() const
{
    return Object::sendMessage<String*>(this, _AK_PRIVATE_SEL(charactersIgnoringModifiers));
}

_AK_INLINE NS::Integer NS::Event::keyCode() const
{
    return Object::sendMessage<Integer>(this, _AK_PRIVATE_SEL(keyCode));
}

_AK_INLINE CGFloat NS::Event::deltaX() const
{
    return Object::sendMessage<CGFloat>(this, _AK_PRIVATE_SEL(deltaX));
}

_AK_INLINE CGFloat NS::Event::deltaY() const
{
    return Object::sendMessage<CGFloat>(this, _AK_PRIVATE_SEL(deltaY));
}

_AK_INLINE NS::TimeInterval NS::Event::timestamp() const
{
    return Object::sendMessage<TimeInterval>(this, _AK_PRIVATE_SEL(timestamp));
}

_AK_INLINE NS::Integer NS::Event::windowNumber() const
{
    return Object::sendMessage<Integer>(this, _AK_PRIVATE_SEL(windowNumber));
}
