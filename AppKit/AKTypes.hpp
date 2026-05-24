//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/AKTypes.hpp
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

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "AKDefines.hpp"
#include "../Foundation/NSTypes.hpp"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CGGeometry.h>

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace NS
{
using Point = CGPoint;
using Size  = CGSize;
using Rect  = CGRect;

struct EdgeInsets
{
    CGFloat top;
    CGFloat left;
    CGFloat bottom;
    CGFloat right;
} _AK_PACKED;

struct DirectionalEdgeInsets
{
    CGFloat top;
    CGFloat leading;
    CGFloat bottom;
    CGFloat trailing;
} _AK_PACKED;

_AK_ENUM(UInteger, RectEdge) {
    RectEdgeMinX = 0,
    RectEdgeMinY = 1,
    RectEdgeMaxX = 2,
    RectEdgeMaxY = 3,
};

using ModalResponse = Integer;

using NSInteger = Integer;
using NSUInteger = UInteger;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
