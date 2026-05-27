//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/AKPrivate.hpp
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
#include <objc/runtime.h>

#define _AK_PRIVATE_CLS(symbol) (AK::Private::Class::s_k##symbol)
#define _AK_PRIVATE_SEL(accessor) (AK::Private::Selector::s_k##accessor)

#if defined(AK_PRIVATE_IMPLEMENTATION)

#include <dlfcn.h>

namespace AK::Private
{
template <typename _Type>
inline _Type const LoadSymbol(const char* pSymbol)
{
    const _Type* pAddress = static_cast<_Type*>(dlsym(RTLD_DEFAULT, pSymbol));

    return pAddress ? *pAddress : _Type();
}
}

#ifdef METALCPP_SYMBOL_VISIBILITY_HIDDEN
#define _AK_PRIVATE_VISIBILITY __attribute__((visibility("hidden")))
#else
#define _AK_PRIVATE_VISIBILITY __attribute__((visibility("default")))
#endif

#ifdef __OBJC__
#define _AK_PRIVATE_OBJC_LOOKUP_CLASS(symbol) ((__bridge void*)objc_lookUpClass(#symbol))
#define _AK_PRIVATE_OBJC_GET_PROTOCOL(symbol) ((__bridge void*)objc_getProtocol(#symbol))
#else
#define _AK_PRIVATE_OBJC_LOOKUP_CLASS(symbol) objc_lookUpClass(#symbol)
#define _AK_PRIVATE_OBJC_GET_PROTOCOL(symbol) objc_getProtocol(#symbol)
#endif

#define _AK_PRIVATE_DEF_CLS(symbol) void* s_k##symbol _AK_PRIVATE_VISIBILITY = _AK_PRIVATE_OBJC_LOOKUP_CLASS(symbol)
#define _AK_PRIVATE_DEF_PRO(symbol) void* s_k##symbol _AK_PRIVATE_VISIBILITY = _AK_PRIVATE_OBJC_GET_PROTOCOL(symbol)
#define _AK_PRIVATE_DEF_SEL(accessor, symbol) SEL s_k##accessor _AK_PRIVATE_VISIBILITY = sel_registerName(symbol)

#define _AK_PRIVATE_DEF_CONST(type, symbol)              \
    _AK_EXTERN type const NS##symbol _AK_PRIVATE_IMPORT; \
    type const                       NS::symbol = (nullptr != &NS##symbol) ? NS##symbol : type()

#define _AK_PRIVATE_DEF_STR(type, symbol)                \
    _AK_EXTERN type const NS##symbol _AK_PRIVATE_IMPORT; \
    type const                       NS::symbol = (nullptr != &NS##symbol) ? NS##symbol : nullptr

#else

#define _AK_PRIVATE_DEF_CLS(symbol) extern void* s_k##symbol
#define _AK_PRIVATE_DEF_PRO(symbol) extern void* s_k##symbol
#define _AK_PRIVATE_DEF_SEL(accessor, symbol) extern SEL s_k##accessor
#define _AK_PRIVATE_DEF_CONST(type, symbol) extern type const NS::symbol
#define _AK_PRIVATE_DEF_STR(type, symbol) extern type const NS::symbol

#endif

namespace AK::Private
{
namespace Class
{
    _AK_PRIVATE_DEF_CLS(NSApplication);
    _AK_PRIVATE_DEF_CLS(NSEvent);
    _AK_PRIVATE_DEF_CLS(NSMenu);
    _AK_PRIVATE_DEF_CLS(NSMenuItem);
    _AK_PRIVATE_DEF_CLS(NSPanel);
    _AK_PRIVATE_DEF_CLS(NSResponder);
    _AK_PRIVATE_DEF_CLS(NSScreen);
    _AK_PRIVATE_DEF_CLS(NSView);
    _AK_PRIVATE_DEF_CLS(NSWindow);
    _AK_PRIVATE_DEF_CLS(NSWindowController);
} // Class

namespace Protocol
{
    _AK_PRIVATE_DEF_PRO(NSApplicationDelegate);
    _AK_PRIVATE_DEF_PRO(NSMenuDelegate);
    _AK_PRIVATE_DEF_PRO(NSWindowDelegate);
} // Protocol

namespace Selector
{
    _AK_PRIVATE_DEF_SEL(abortModal, "abortModal");
    _AK_PRIVATE_DEF_SEL(acceptsFirstResponder, "acceptsFirstResponder");
    _AK_PRIVATE_DEF_SEL(acceptsMouseMovedEvents, "acceptsMouseMovedEvents");
    _AK_PRIVATE_DEF_SEL(action, "action");
    _AK_PRIVATE_DEF_SEL(activate, "activate");
    _AK_PRIVATE_DEF_SEL(activateIgnoringOtherApps_, "activateIgnoringOtherApps:");
    _AK_PRIVATE_DEF_SEL(activationPolicy, "activationPolicy");
    _AK_PRIVATE_DEF_SEL(active, "isActive");
    _AK_PRIVATE_DEF_SEL(addItem_, "addItem:");
    _AK_PRIVATE_DEF_SEL(addItemWithTitle_action_keyEquivalent_, "addItemWithTitle:action:keyEquivalent:");
    _AK_PRIVATE_DEF_SEL(addSubview_, "addSubview:");
    _AK_PRIVATE_DEF_SEL(alphaValue, "alphaValue");
    _AK_PRIVATE_DEF_SEL(autoenablesItems, "autoenablesItems");
    _AK_PRIVATE_DEF_SEL(autoresizingMask, "autoresizingMask");
    _AK_PRIVATE_DEF_SEL(backingScaleFactor, "backingScaleFactor");
    _AK_PRIVATE_DEF_SEL(becomeFirstResponder, "becomeFirstResponder");
    _AK_PRIVATE_DEF_SEL(bounds, "bounds");
    _AK_PRIVATE_DEF_SEL(buttonNumber, "buttonNumber");
    _AK_PRIVATE_DEF_SEL(center, "center");
    _AK_PRIVATE_DEF_SEL(characters, "characters");
    _AK_PRIVATE_DEF_SEL(charactersIgnoringModifiers, "charactersIgnoringModifiers");
    _AK_PRIVATE_DEF_SEL(clickCount, "clickCount");
    _AK_PRIVATE_DEF_SEL(close, "close");
    _AK_PRIVATE_DEF_SEL(collectionBehavior, "collectionBehavior");
    _AK_PRIVATE_DEF_SEL(contentAspectRatio, "contentAspectRatio");
    _AK_PRIVATE_DEF_SEL(contentSize, "contentSize");
    _AK_PRIVATE_DEF_SEL(contentView, "contentView");
    _AK_PRIVATE_DEF_SEL(convertPoint_fromView_, "convertPoint:fromView:");
    _AK_PRIVATE_DEF_SEL(convertPoint_toView_, "convertPoint:toView:");
    _AK_PRIVATE_DEF_SEL(convertRectFromScreen_, "convertRectFromScreen:");
    _AK_PRIVATE_DEF_SEL(convertRectToScreen_, "convertRectToScreen:");
    _AK_PRIVATE_DEF_SEL(currentEvent, "currentEvent");
    _AK_PRIVATE_DEF_SEL(deltaX, "deltaX");
    _AK_PRIVATE_DEF_SEL(deltaY, "deltaY");
    _AK_PRIVATE_DEF_SEL(delegate, "delegate");
    _AK_PRIVATE_DEF_SEL(display, "display");
    _AK_PRIVATE_DEF_SEL(displayIfNeeded, "displayIfNeeded");
    _AK_PRIVATE_DEF_SEL(deminiaturize_, "deminiaturize:");
    _AK_PRIVATE_DEF_SEL(deviceDescription, "deviceDescription");
    _AK_PRIVATE_DEF_SEL(finishLaunching, "finishLaunching");
    _AK_PRIVATE_DEF_SEL(flagsChanged_, "flagsChanged:");
    _AK_PRIVATE_DEF_SEL(frame, "frame");
    _AK_PRIVATE_DEF_SEL(hasPreciseScrollingDeltas, "hasPreciseScrollingDeltas");
    _AK_PRIVATE_DEF_SEL(hasShadow, "hasShadow");
    _AK_PRIVATE_DEF_SEL(helpMenu, "helpMenu");
    _AK_PRIVATE_DEF_SEL(hidden, "isHidden");
    _AK_PRIVATE_DEF_SEL(hide_, "hide:");
    _AK_PRIVATE_DEF_SEL(hideOtherApplications_, "hideOtherApplications:");
    _AK_PRIVATE_DEF_SEL(hitTest_, "hitTest:");
    _AK_PRIVATE_DEF_SEL(init, "init");
    _AK_PRIVATE_DEF_SEL(initWithCoder_, "initWithCoder:");
    _AK_PRIVATE_DEF_SEL(initWithContentRect_styleMask_backing_defer_, "initWithContentRect:styleMask:backing:defer:");
    _AK_PRIVATE_DEF_SEL(initWithFrame_, "initWithFrame:");
    _AK_PRIVATE_DEF_SEL(initWithTitle_, "initWithTitle:");
    _AK_PRIVATE_DEF_SEL(initWithTitle_action_keyEquivalent_, "initWithTitle:action:keyEquivalent:");
    _AK_PRIVATE_DEF_SEL(initWithWindow_, "initWithWindow:");
    _AK_PRIVATE_DEF_SEL(insertItem_atIndex_, "insertItem:atIndex:");
    _AK_PRIVATE_DEF_SEL(ignoresMouseEvents, "ignoresMouseEvents");
    _AK_PRIVATE_DEF_SEL(isEnabled, "isEnabled");
    _AK_PRIVATE_DEF_SEL(isMovable, "isMovable");
    _AK_PRIVATE_DEF_SEL(isOpaque, "isOpaque");
    _AK_PRIVATE_DEF_SEL(isReleasedWhenClosed, "isReleasedWhenClosed");
    _AK_PRIVATE_DEF_SEL(isSeparatorItem, "isSeparatorItem");
    _AK_PRIVATE_DEF_SEL(isVisible, "isVisible");
    _AK_PRIVATE_DEF_SEL(isFloatingPanel, "isFloatingPanel");
    _AK_PRIVATE_DEF_SEL(isKeyWindow, "isKeyWindow");
    _AK_PRIVATE_DEF_SEL(isMainWindow, "isMainWindow");
    _AK_PRIVATE_DEF_SEL(isMiniaturized, "isMiniaturized");
    _AK_PRIVATE_DEF_SEL(isRunning, "isRunning");
    _AK_PRIVATE_DEF_SEL(isZoomed, "isZoomed");
    _AK_PRIVATE_DEF_SEL(itemAtIndex_, "itemAtIndex:");
    _AK_PRIVATE_DEF_SEL(itemWithTag_, "itemWithTag:");
    _AK_PRIVATE_DEF_SEL(itemWithTitle_, "itemWithTitle:");
    _AK_PRIVATE_DEF_SEL(keyCode, "keyCode");
    _AK_PRIVATE_DEF_SEL(keyDown_, "keyDown:");
    _AK_PRIVATE_DEF_SEL(keyEventWithType_location_modifierFlags_timestamp_windowNumber_context_characters_charactersIgnoringModifiers_isARepeat_keyCode_, "keyEventWithType:location:modifierFlags:timestamp:windowNumber:context:characters:charactersIgnoringModifiers:isARepeat:keyCode:");
    _AK_PRIVATE_DEF_SEL(keyEquivalent, "keyEquivalent");
    _AK_PRIVATE_DEF_SEL(keyEquivalentModifierMask, "keyEquivalentModifierMask");
    _AK_PRIVATE_DEF_SEL(keyUp_, "keyUp:");
    _AK_PRIVATE_DEF_SEL(keyWindow, "keyWindow");
    _AK_PRIVATE_DEF_SEL(layer, "layer");
    _AK_PRIVATE_DEF_SEL(level, "level");
    _AK_PRIVATE_DEF_SEL(locationInWindow, "locationInWindow");
    _AK_PRIVATE_DEF_SEL(mainMenu, "mainMenu");
    _AK_PRIVATE_DEF_SEL(mainScreen, "mainScreen");
    _AK_PRIVATE_DEF_SEL(mainWindow, "mainWindow");
    _AK_PRIVATE_DEF_SEL(makeKeyAndOrderFront_, "makeKeyAndOrderFront:");
    _AK_PRIVATE_DEF_SEL(makeKeyWindow, "makeKeyWindow");
    _AK_PRIVATE_DEF_SEL(makeMainWindow, "makeMainWindow");
    _AK_PRIVATE_DEF_SEL(maxSize, "maxSize");
    _AK_PRIVATE_DEF_SEL(menu, "menu");
    _AK_PRIVATE_DEF_SEL(miniaturize_, "miniaturize:");
    _AK_PRIVATE_DEF_SEL(minSize, "minSize");
    _AK_PRIVATE_DEF_SEL(modifierFlags, "modifierFlags");
    _AK_PRIVATE_DEF_SEL(mouseDown_, "mouseDown:");
    _AK_PRIVATE_DEF_SEL(mouseEntered_, "mouseEntered:");
    _AK_PRIVATE_DEF_SEL(mouseEventWithType_location_modifierFlags_timestamp_windowNumber_context_eventNumber_clickCount_pressure_, "mouseEventWithType:location:modifierFlags:timestamp:windowNumber:context:eventNumber:clickCount:pressure:");
    _AK_PRIVATE_DEF_SEL(mouseExited_, "mouseExited:");
    _AK_PRIVATE_DEF_SEL(mouseMoved_, "mouseMoved:");
    _AK_PRIVATE_DEF_SEL(mouseUp_, "mouseUp:");
    _AK_PRIVATE_DEF_SEL(nextEventMatchingMask_untilDate_inMode_dequeue_, "nextEventMatchingMask:untilDate:inMode:dequeue:");
    _AK_PRIVATE_DEF_SEL(nextResponder, "nextResponder");
    _AK_PRIVATE_DEF_SEL(numberOfItems, "numberOfItems");
    _AK_PRIVATE_DEF_SEL(orderFrontRegardless, "orderFrontRegardless");
    _AK_PRIVATE_DEF_SEL(orderFront_, "orderFront:");
    _AK_PRIVATE_DEF_SEL(orderOut_, "orderOut:");
    _AK_PRIVATE_DEF_SEL(otherMouseDown_, "otherMouseDown:");
    _AK_PRIVATE_DEF_SEL(otherMouseUp_, "otherMouseUp:");
    _AK_PRIVATE_DEF_SEL(performActionForItemAtIndex_, "performActionForItemAtIndex:");
    _AK_PRIVATE_DEF_SEL(performKeyEquivalent_, "performKeyEquivalent:");
    _AK_PRIVATE_DEF_SEL(phase, "phase");
    _AK_PRIVATE_DEF_SEL(popUpMenuPositioningItem_atLocation_inView_, "popUpMenuPositioningItem:atLocation:inView:");
    _AK_PRIVATE_DEF_SEL(postEvent_atStart_, "postEvent:atStart:");
    _AK_PRIVATE_DEF_SEL(presentationOptions, "presentationOptions");
    _AK_PRIVATE_DEF_SEL(removeAllItems, "removeAllItems");
    _AK_PRIVATE_DEF_SEL(removeFromSuperview, "removeFromSuperview");
    _AK_PRIVATE_DEF_SEL(removeItem_, "removeItem:");
    _AK_PRIVATE_DEF_SEL(removeItemAtIndex_, "removeItemAtIndex:");
    _AK_PRIVATE_DEF_SEL(representedObject, "representedObject");
    _AK_PRIVATE_DEF_SEL(resignFirstResponder, "resignFirstResponder");
    _AK_PRIVATE_DEF_SEL(rightMouseDown_, "rightMouseDown:");
    _AK_PRIVATE_DEF_SEL(rightMouseUp_, "rightMouseUp:");
    _AK_PRIVATE_DEF_SEL(scrollingDeltaX, "scrollingDeltaX");
    _AK_PRIVATE_DEF_SEL(scrollingDeltaY, "scrollingDeltaY");
    _AK_PRIVATE_DEF_SEL(resignKeyWindow, "resignKeyWindow");
    _AK_PRIVATE_DEF_SEL(resignMainWindow, "resignMainWindow");
    _AK_PRIVATE_DEF_SEL(run, "run");
    _AK_PRIVATE_DEF_SEL(screen, "screen");
    _AK_PRIVATE_DEF_SEL(screens, "screens");
    _AK_PRIVATE_DEF_SEL(scrollWheel_, "scrollWheel:");
    _AK_PRIVATE_DEF_SEL(sendAction_to_from_, "sendAction:to:from:");
    _AK_PRIVATE_DEF_SEL(sendEvent_, "sendEvent:");
    _AK_PRIVATE_DEF_SEL(separatorItem, "separatorItem");
    _AK_PRIVATE_DEF_SEL(servicesMenu, "servicesMenu");
    _AK_PRIVATE_DEF_SEL(setAcceptsMouseMovedEvents_, "setAcceptsMouseMovedEvents:");
    _AK_PRIVATE_DEF_SEL(setAction_, "setAction:");
    _AK_PRIVATE_DEF_SEL(setAlphaValue_, "setAlphaValue:");
    _AK_PRIVATE_DEF_SEL(setActivationPolicy_, "setActivationPolicy:");
    _AK_PRIVATE_DEF_SEL(setAutoresizingMask_, "setAutoresizingMask:");
    _AK_PRIVATE_DEF_SEL(setAutoenablesItems_, "setAutoenablesItems:");
    _AK_PRIVATE_DEF_SEL(setBounds_, "setBounds:");
    _AK_PRIVATE_DEF_SEL(setCollectionBehavior_, "setCollectionBehavior:");
    _AK_PRIVATE_DEF_SEL(setContentAspectRatio_, "setContentAspectRatio:");
    _AK_PRIVATE_DEF_SEL(setContentSize_, "setContentSize:");
    _AK_PRIVATE_DEF_SEL(setContentView_, "setContentView:");
    _AK_PRIVATE_DEF_SEL(setDelegate_, "setDelegate:");
    _AK_PRIVATE_DEF_SEL(setEnabled_, "setEnabled:");
    _AK_PRIVATE_DEF_SEL(setFloatingPanel_, "setFloatingPanel:");
    _AK_PRIVATE_DEF_SEL(setFrame_, "setFrame:");
    _AK_PRIVATE_DEF_SEL(setFrameSize_, "setFrameSize:");
    _AK_PRIVATE_DEF_SEL(setFrame_display_, "setFrame:display:");
    _AK_PRIVATE_DEF_SEL(setHasShadow_, "setHasShadow:");
    _AK_PRIVATE_DEF_SEL(setHelpMenu_, "setHelpMenu:");
    _AK_PRIVATE_DEF_SEL(setHidden_, "setHidden:");
    _AK_PRIVATE_DEF_SEL(setIgnoresMouseEvents_, "setIgnoresMouseEvents:");
    _AK_PRIVATE_DEF_SEL(setKeyEquivalentModifierMask_, "setKeyEquivalentModifierMask:");
    _AK_PRIVATE_DEF_SEL(setKeyEquivalent_, "setKeyEquivalent:");
    _AK_PRIVATE_DEF_SEL(setLevel_, "setLevel:");
    _AK_PRIVATE_DEF_SEL(setLayer_, "setLayer:");
    _AK_PRIVATE_DEF_SEL(setMainMenu_, "setMainMenu:");
    _AK_PRIVATE_DEF_SEL(setMaxSize_, "setMaxSize:");
    _AK_PRIVATE_DEF_SEL(setMinSize_, "setMinSize:");
    _AK_PRIVATE_DEF_SEL(setMovable_, "setMovable:");
    _AK_PRIVATE_DEF_SEL(setNeedsDisplay_, "setNeedsDisplay:");
    _AK_PRIVATE_DEF_SEL(setNextResponder_, "setNextResponder:");
    _AK_PRIVATE_DEF_SEL(setOpaque_, "setOpaque:");
    _AK_PRIVATE_DEF_SEL(setPresentationOptions_, "setPresentationOptions:");
    _AK_PRIVATE_DEF_SEL(setReleasedWhenClosed_, "setReleasedWhenClosed:");
    _AK_PRIVATE_DEF_SEL(setRepresentedObject_, "setRepresentedObject:");
    _AK_PRIVATE_DEF_SEL(setServicesMenu_, "setServicesMenu:");
    _AK_PRIVATE_DEF_SEL(setStyleMask_, "setStyleMask:");
    _AK_PRIVATE_DEF_SEL(setState_, "setState:");
    _AK_PRIVATE_DEF_SEL(setSubmenu_, "setSubmenu:");
    _AK_PRIVATE_DEF_SEL(setTag_, "setTag:");
    _AK_PRIVATE_DEF_SEL(setTarget_, "setTarget:");
    _AK_PRIVATE_DEF_SEL(setTitle_, "setTitle:");
    _AK_PRIVATE_DEF_SEL(setUndoManager_, "setUndoManager:");
    _AK_PRIVATE_DEF_SEL(setWantsLayer_, "setWantsLayer:");
    _AK_PRIVATE_DEF_SEL(setWindow_, "setWindow:");
    _AK_PRIVATE_DEF_SEL(setWorksWhenModal_, "setWorksWhenModal:");
    _AK_PRIVATE_DEF_SEL(sharedApplication, "sharedApplication");
    _AK_PRIVATE_DEF_SEL(showWindow_, "showWindow:");
    _AK_PRIVATE_DEF_SEL(standardWindowButton_, "standardWindowButton:");
    _AK_PRIVATE_DEF_SEL(state, "state");
    _AK_PRIVATE_DEF_SEL(stop_, "stop:");
    _AK_PRIVATE_DEF_SEL(styleMask, "styleMask");
    _AK_PRIVATE_DEF_SEL(submenu, "submenu");
    _AK_PRIVATE_DEF_SEL(subviews, "subviews");
    _AK_PRIVATE_DEF_SEL(superview, "superview");
    _AK_PRIVATE_DEF_SEL(tag, "tag");
    _AK_PRIVATE_DEF_SEL(target, "target");
    _AK_PRIVATE_DEF_SEL(terminate_, "terminate:");
    _AK_PRIVATE_DEF_SEL(timestamp, "timestamp");
    _AK_PRIVATE_DEF_SEL(title, "title");
    _AK_PRIVATE_DEF_SEL(toggleFullScreen_, "toggleFullScreen:");
    _AK_PRIVATE_DEF_SEL(tryToPerform_with_, "tryToPerform:with:");
    _AK_PRIVATE_DEF_SEL(type, "type");
    _AK_PRIVATE_DEF_SEL(unhideAllApplications_, "unhideAllApplications:");
    _AK_PRIVATE_DEF_SEL(unhide_, "unhide:");
    _AK_PRIVATE_DEF_SEL(unhideWithoutActivation, "unhideWithoutActivation");
    _AK_PRIVATE_DEF_SEL(undoManager, "undoManager");
    _AK_PRIVATE_DEF_SEL(updateWindows, "updateWindows");
    _AK_PRIVATE_DEF_SEL(validateMenuItem_, "validateMenuItem:");
    _AK_PRIVATE_DEF_SEL(visibleFrame, "visibleFrame");
    _AK_PRIVATE_DEF_SEL(wantsLayer, "wantsLayer");
    _AK_PRIVATE_DEF_SEL(window, "window");
    _AK_PRIVATE_DEF_SEL(windowNumber, "windowNumber");
    _AK_PRIVATE_DEF_SEL(windows, "windows");
    _AK_PRIVATE_DEF_SEL(worksWhenModal, "worksWhenModal");
} // Selector
} // Private
