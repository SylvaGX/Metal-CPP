//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// ModelIO/MDLAsset.hpp
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

#include "../Foundation/Foundation.hpp"
#include "MDLDefines.hpp"
#include "MDLMeshBufferAllocator.hpp"
#include "MDLPrivate.hpp"
#include "MDLTypes.hpp"
#include "MDLVertexDescriptor.hpp"

namespace MDL
{
class Asset : public NS::Copying<Asset>
{
public:
    static Asset* alloc();

    Asset* initWithURL(const NS::URL* pURL);

    Asset* initWithURL(const NS::URL* pURL, VertexDescriptor* pVertexDescriptor, MeshBufferAllocator* pBufferAllocator);
};
} // MDL

_MDL_INLINE MDL::Asset* MDL::Asset::alloc()
{
    return NS::Object::alloc<Asset>(_MDL_PRIVATE_CLS(MDLAsset));
}

_MDL_INLINE MDL::Asset* MDL::Asset::initWithURL(const NS::URL* pURL)
{
    return Object::sendMessage<Asset*>(this, _MDL_PRIVATE_SEL(initWithURL_), pURL);
}

_MDL_INLINE MDL::Asset* MDL::Asset::initWithURL(const NS::URL* pURL, VertexDescriptor* pVertexDescriptor, MeshBufferAllocator* pBufferAllocator)
{
    return Object::sendMessage<Asset*>(this, _MDL_PRIVATE_SEL(initWithURL_vertexDescriptor_bufferAllocator_), pURL, pVertexDescriptor, pBufferAllocator);
}
