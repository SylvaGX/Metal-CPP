//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/AKGeometry.hpp
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

#include "AKTypes.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace NS
{
_AK_INLINE Point MakePoint(CGFloat x, CGFloat y)
{
    return CGPointMake(x, y);
}

_AK_INLINE Size MakeSize(CGFloat width, CGFloat height)
{
    return CGSizeMake(width, height);
}

_AK_INLINE Rect MakeRect(CGFloat x, CGFloat y, CGFloat width, CGFloat height)
{
    return CGRectMake(x, y, width, height);
}

_AK_INLINE Rect MakeRect(const Point& origin, const Size& size)
{
    return CGRectMake(origin.x, origin.y, size.width, size.height);
}

_AK_INLINE bool EqualPoints(const Point& point1, const Point& point2)
{
    return CGPointEqualToPoint(point1, point2);
}

_AK_INLINE bool EqualSizes(const Size& size1, const Size& size2)
{
    return CGSizeEqualToSize(size1, size2);
}

_AK_INLINE bool EqualRects(const Rect& rect1, const Rect& rect2)
{
    return CGRectEqualToRect(rect1, rect2);
}

_AK_INLINE bool IsEmptyRect(const Rect& rect)
{
    return CGRectIsEmpty(rect);
}

_AK_INLINE bool IsNullRect(const Rect& rect)
{
    return CGRectIsNull(rect);
}

_AK_INLINE Rect InsetRect(const Rect& rect, CGFloat dx, CGFloat dy)
{
    return CGRectInset(rect, dx, dy);
}

_AK_INLINE Rect OffsetRect(const Rect& rect, CGFloat dx, CGFloat dy)
{
    return CGRectOffset(rect, dx, dy);
}

_AK_INLINE Rect UnionRect(const Rect& rect1, const Rect& rect2)
{
    return CGRectUnion(rect1, rect2);
}

_AK_INLINE Rect IntersectionRect(const Rect& rect1, const Rect& rect2)
{
    return CGRectIntersection(rect1, rect2);
}

_AK_INLINE bool ContainsPoint(const Rect& rect, const Point& point)
{
    return CGRectContainsPoint(rect, point);
}

_AK_INLINE bool ContainsRect(const Rect& rect1, const Rect& rect2)
{
    return CGRectContainsRect(rect1, rect2);
}

_AK_INLINE Point CenterPoint(const Rect& rect)
{
    return CGPointMake(CGRectGetMidX(rect), CGRectGetMidY(rect));
}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
