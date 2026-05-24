//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/AKAppKitEnums.hpp
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

#include "AKDefines.hpp"
#include "../Foundation/NSTypes.hpp"

namespace NS
{
_AK_ENUM(Integer, ApplicationActivationPolicy) {
    ApplicationActivationPolicyRegular    = 0,
    ApplicationActivationPolicyAccessory  = 1,
    ApplicationActivationPolicyProhibited = 2,
};

_AK_ENUM(Integer, ApplicationTerminateReply) {
    ApplicationTerminateCancel     = 0,
    ApplicationTerminateNow        = 1,
    ApplicationTerminateLater      = 2,
};

_AK_ENUM(UInteger, EventType) {
    EventTypeLeftMouseDown    = 1,
    EventTypeLeftMouseUp      = 2,
    EventTypeRightMouseDown   = 3,
    EventTypeRightMouseUp     = 4,
    EventTypeMouseMoved       = 5,
    EventTypeLeftMouseDragged = 6,
    EventTypeRightMouseDragged = 7,
    EventTypeMouseEntered     = 8,
    EventTypeMouseExited      = 9,
    EventTypeKeyDown          = 10,
    EventTypeKeyUp            = 11,
    EventTypeFlagsChanged     = 12,
    EventTypeAppKitDefined    = 13,
    EventTypeSystemDefined    = 14,
    EventTypeApplicationDefined = 15,
    EventTypePeriodic         = 16,
    EventTypeCursorUpdate     = 17,
    EventTypeScrollWheel      = 22,
    EventTypeTabletPoint      = 23,
    EventTypeTabletProximity  = 24,
    EventTypeOtherMouseDown   = 25,
    EventTypeOtherMouseUp     = 26,
    EventTypeOtherMouseDragged = 27,
    EventTypeGesture          = 29,
    EventTypePressure         = 30,
    EventTypeDirectTouch      = 31,
};

_AK_OPTIONS(UInteger, EventMask) {
    EventMaskLeftMouseDown        = 1ULL << EventTypeLeftMouseDown,
    EventMaskLeftMouseUp          = 1ULL << EventTypeLeftMouseUp,
    EventMaskRightMouseDown       = 1ULL << EventTypeRightMouseDown,
    EventMaskRightMouseUp         = 1ULL << EventTypeRightMouseUp,
    EventMaskMouseMoved           = 1ULL << EventTypeMouseMoved,
    EventMaskLeftMouseDragged     = 1ULL << EventTypeLeftMouseDragged,
    EventMaskRightMouseDragged    = 1ULL << EventTypeRightMouseDragged,
    EventMaskMouseEntered         = 1ULL << EventTypeMouseEntered,
    EventMaskMouseExited          = 1ULL << EventTypeMouseExited,
    EventMaskKeyDown              = 1ULL << EventTypeKeyDown,
    EventMaskKeyUp                = 1ULL << EventTypeKeyUp,
    EventMaskFlagsChanged         = 1ULL << EventTypeFlagsChanged,
    EventMaskAppKitDefined        = 1ULL << EventTypeAppKitDefined,
    EventMaskSystemDefined        = 1ULL << EventTypeSystemDefined,
    EventMaskApplicationDefined   = 1ULL << EventTypeApplicationDefined,
    EventMaskPeriodic             = 1ULL << EventTypePeriodic,
    EventMaskCursorUpdate         = 1ULL << EventTypeCursorUpdate,
    EventMaskScrollWheel          = 1ULL << EventTypeScrollWheel,
    EventMaskTabletPoint          = 1ULL << EventTypeTabletPoint,
    EventMaskTabletProximity      = 1ULL << EventTypeTabletProximity,
    EventMaskOtherMouseDown       = 1ULL << EventTypeOtherMouseDown,
    EventMaskOtherMouseUp         = 1ULL << EventTypeOtherMouseUp,
    EventMaskOtherMouseDragged    = 1ULL << EventTypeOtherMouseDragged,
    EventMaskGesture              = 1ULL << EventTypeGesture,
    EventMaskPressure             = 1ULL << EventTypePressure,
    EventMaskDirectTouch          = 1ULL << EventTypeDirectTouch,
    EventMaskAny                  = ~0ULL,
};

_AK_OPTIONS(UInteger, EventModifierFlags) {
    EventModifierFlagCapsLock   = 1 << 16,
    EventModifierFlagShift      = 1 << 17,
    EventModifierFlagControl    = 1 << 18,
    EventModifierFlagOption     = 1 << 19,
    EventModifierFlagCommand    = 1 << 20,
    EventModifierFlagNumericPad = 1 << 21,
    EventModifierFlagHelp       = 1 << 22,
    EventModifierFlagFunction   = 1 << 23,
    EventModifierFlagDeviceIndependentFlagsMask = 0xffff0000U,
};

_AK_OPTIONS(UInteger, WindowStyleMask) {
    WindowStyleMaskBorderless          = 0,
    WindowStyleMaskTitled              = 1 << 0,
    WindowStyleMaskClosable            = 1 << 1,
    WindowStyleMaskMiniaturizable      = 1 << 2,
    WindowStyleMaskResizable            = 1 << 3,
    WindowStyleMaskFullSizeContentView = 1 << 15,
};

_AK_ENUM(UInteger, BackingStoreType) {
    BackingStoreRetained    = 0,
    BackingStoreNonretained = 1,
    BackingStoreBuffered    = 2,
};

_AK_ENUM(Integer, WindowLevel) {
    WindowLevelNormal           = 0,
    WindowLevelFloating         = 3,
    WindowLevelSubmenu          = 3,
    WindowLevelTornOffMenu      = 3,
    WindowLevelMainMenu         = 24,
    WindowLevelStatusBar        = 25,
    WindowLevelModalPanel       = 8,
    WindowLevelPopUpMenu        = 101,
    WindowLevelScreenSaver      = 1000,
};

_AK_OPTIONS(UInteger, AutoresizingMaskOptions) {
    ViewNotSizable    = 0,
    ViewMinXMargin    = 1,
    ViewWidthSizable  = 2,
    ViewMaxXMargin    = 4,
    ViewMinYMargin    = 8,
    ViewHeightSizable = 16,
    ViewMaxYMargin    = 32,
};

_AK_ENUM(Integer, PanelStyle) {
    PanelStyleNonactivating = 128,
};

_AK_OPTIONS(UInteger, WindowCollectionBehavior) {
    WindowCollectionBehaviorDefault              = 0,
    WindowCollectionBehaviorCanJoinAllSpaces     = 1 << 0,
    WindowCollectionBehaviorMoveToActiveSpace    = 1 << 1,
    WindowCollectionBehaviorManaged              = 1 << 2,
    WindowCollectionBehaviorParticipatesInCycle  = 1 << 3,
    WindowCollectionBehaviorIgnoresCycle           = 1 << 4,
    WindowCollectionBehaviorFullScreenPrimary    = 1 << 7,
    WindowCollectionBehaviorFullScreenAuxiliary  = 1 << 8,
    WindowCollectionBehaviorFullScreenAllowsTiling = 1 << 11,
    WindowCollectionBehaviorFullScreenDisallowsTiling = 1 << 12,
};
}
