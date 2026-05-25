//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// ModelIO/MDLEnums.hpp
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

#include "../Foundation/NSDefines.hpp"
#include "../Foundation/NSTypes.hpp"

#include "MDLDefines.hpp"

namespace MDL
{
_MDL_ENUM(NS::UInteger, MeshBufferType) {
    MeshBufferTypeVertex = 1,
    MeshBufferTypeIndex = 2,
    MeshBufferTypeCustom = 3,
};

_MDL_ENUM(NS::UInteger, VertexFormat) {
    VertexFormatInvalid = 0,

    VertexFormatPackedBit = 0x1000,

    VertexFormatUCharBits = 0x10000,
    VertexFormatCharBits = 0x20000,
    VertexFormatUCharNormalizedBits = 0x30000,
    VertexFormatCharNormalizedBits = 0x40000,
    VertexFormatUShortBits = 0x50000,
    VertexFormatShortBits = 0x60000,
    VertexFormatUShortNormalizedBits = 0x70000,
    VertexFormatShortNormalizedBits = 0x80000,
    VertexFormatUIntBits = 0x90000,
    VertexFormatIntBits = 0xA0000,
    VertexFormatHalfBits = 0xB0000,
    VertexFormatFloatBits = 0xC0000,

    VertexFormatUChar = VertexFormatUCharBits | 1,
    VertexFormatUChar2 = VertexFormatUCharBits | 2,
    VertexFormatUChar3 = VertexFormatUCharBits | 3,
    VertexFormatUChar4 = VertexFormatUCharBits | 4,

    VertexFormatChar = VertexFormatCharBits | 1,
    VertexFormatChar2 = VertexFormatCharBits | 2,
    VertexFormatChar3 = VertexFormatCharBits | 3,
    VertexFormatChar4 = VertexFormatCharBits | 4,

    VertexFormatUCharNormalized = VertexFormatUCharNormalizedBits | 1,
    VertexFormatUChar2Normalized = VertexFormatUCharNormalizedBits | 2,
    VertexFormatUChar3Normalized = VertexFormatUCharNormalizedBits | 3,
    VertexFormatUChar4Normalized = VertexFormatUCharNormalizedBits | 4,

    VertexFormatCharNormalized = VertexFormatCharNormalizedBits | 1,
    VertexFormatChar2Normalized = VertexFormatCharNormalizedBits | 2,
    VertexFormatChar3Normalized = VertexFormatCharNormalizedBits | 3,
    VertexFormatChar4Normalized = VertexFormatCharNormalizedBits | 4,

    VertexFormatUShort = VertexFormatUShortBits | 1,
    VertexFormatUShort2 = VertexFormatUShortBits | 2,
    VertexFormatUShort3 = VertexFormatUShortBits | 3,
    VertexFormatUShort4 = VertexFormatUShortBits | 4,

    VertexFormatShort = VertexFormatShortBits | 1,
    VertexFormatShort2 = VertexFormatShortBits | 2,
    VertexFormatShort3 = VertexFormatShortBits | 3,
    VertexFormatShort4 = VertexFormatShortBits | 4,

    VertexFormatUShortNormalized = VertexFormatUShortNormalizedBits | 1,
    VertexFormatUShort2Normalized = VertexFormatUShortNormalizedBits | 2,
    VertexFormatUShort3Normalized = VertexFormatUShortNormalizedBits | 3,
    VertexFormatUShort4Normalized = VertexFormatUShortNormalizedBits | 4,

    VertexFormatShortNormalized = VertexFormatShortNormalizedBits | 1,
    VertexFormatShort2Normalized = VertexFormatShortNormalizedBits | 2,
    VertexFormatShort3Normalized = VertexFormatShortNormalizedBits | 3,
    VertexFormatShort4Normalized = VertexFormatShortNormalizedBits | 4,

    VertexFormatUInt = VertexFormatUIntBits | 1,
    VertexFormatUInt2 = VertexFormatUIntBits | 2,
    VertexFormatUInt3 = VertexFormatUIntBits | 3,
    VertexFormatUInt4 = VertexFormatUIntBits | 4,

    VertexFormatInt = VertexFormatIntBits | 1,
    VertexFormatInt2 = VertexFormatIntBits | 2,
    VertexFormatInt3 = VertexFormatIntBits | 3,
    VertexFormatInt4 = VertexFormatIntBits | 4,

    VertexFormatHalf = VertexFormatHalfBits | 1,
    VertexFormatHalf2 = VertexFormatHalfBits | 2,
    VertexFormatHalf3 = VertexFormatHalfBits | 3,
    VertexFormatHalf4 = VertexFormatHalfBits | 4,

    VertexFormatFloat = VertexFormatFloatBits | 1,
    VertexFormatFloat2 = VertexFormatFloatBits | 2,
    VertexFormatFloat3 = VertexFormatFloatBits | 3,
    VertexFormatFloat4 = VertexFormatFloatBits | 4,

    VertexFormatInt1010102Normalized = VertexFormatIntBits | VertexFormatPackedBit | 4,
    VertexFormatUInt1010102Normalized = VertexFormatUIntBits | VertexFormatPackedBit | 4,
};
} // MDL
