//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// MetalKit/MTKModel.hpp
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
#include "../Metal/MTLArgument.hpp"
#include "../Metal/MTLBuffer.hpp"
#include "../Metal/MTLRenderCommandEncoder.hpp"
#include "../Metal/MTLVertexDescriptor.hpp"
#include "MTKDefines.hpp"
#include "MTKPrivate.hpp"
#include "MTKTypes.hpp"

extern "C" void* MTKModelIOVertexDescriptorFromMetal(void* metalDescriptor);
extern "C" void* MTKModelIOVertexDescriptorFromMetalWithError(void* metalDescriptor, void** error);
extern "C" void* MTKMetalVertexDescriptorFromModelIO(void* modelIODescriptor);
extern "C" void* MTKMetalVertexDescriptorFromModelIOWithError(void* modelIODescriptor, void** error);
extern "C" NS::UInteger MTKModelIOVertexFormatFromMetal(NS::UInteger vertexFormat);
extern "C" NS::UInteger MTKMetalVertexFormatFromModelIO(NS::UInteger vertexFormat);

namespace MTK
{
using ModelError = NS::String*;

_MTK_CONST(ModelError, ModelErrorDomain);
_MTK_CONST(ModelError, ModelErrorKey);

class MeshBufferAllocator;
class MeshBuffer;
class Submesh;

class MeshBufferAllocator : public NS::Referencing<MeshBufferAllocator>
{
public:
    static MeshBufferAllocator* alloc();

    MeshBufferAllocator* initWithDevice(MTL::Device* pDevice);

    MTL::Device* device() const;
};

class MeshBuffer : public NS::Referencing<MeshBuffer>
{
public:
    NS::UInteger         length() const;
    MeshBufferAllocator* allocator() const;
    MDL::MeshBufferZone* zone() const;
    MTL::Buffer*         buffer() const;
    NS::UInteger         offset() const;
    MDL::MeshBufferType  type() const;
};

class Submesh : public NS::Referencing<Submesh>
{
public:
    MTL::PrimitiveType primitiveType() const;
    MTL::IndexType     indexType() const;
    MeshBuffer*        indexBuffer() const;
    NS::UInteger       indexCount() const;
    class Mesh*        mesh() const;
    NS::String*        name() const;
    void               setName(const NS::String* pName);
};

class Mesh : public NS::Referencing<Mesh>
{
public:
    static Mesh* alloc();

    Mesh* initWithMesh(MDL::Mesh* pMesh, MTL::Device* pDevice, NS::Error** ppError);

    static NS::Array* newMeshesFromAsset(MDL::Asset* pAsset, MTL::Device* pDevice, NS::Array** ppSourceMeshes, NS::Error** ppError);

    NS::Array*              vertexBuffers() const;
    MDL::VertexDescriptor*  vertexDescriptor() const;
    NS::Array*              submeshes() const;
    NS::UInteger            vertexCount() const;
    NS::String*             name() const;
    void                    setName(const NS::String* pName);
};

MDL::VertexDescriptor* ModelIOVertexDescriptorFromMetal(MTL::VertexDescriptor* pDescriptor);
MDL::VertexDescriptor* ModelIOVertexDescriptorFromMetalWithError(MTL::VertexDescriptor* pDescriptor, NS::Error** ppError);
MTL::VertexDescriptor* MetalVertexDescriptorFromModelIO(MDL::VertexDescriptor* pDescriptor);
MTL::VertexDescriptor* MetalVertexDescriptorFromModelIOWithError(MDL::VertexDescriptor* pDescriptor, NS::Error** ppError);
MDL::VertexFormat      ModelIOVertexFormatFromMetal(MTL::VertexFormat vertexFormat);
MTL::VertexFormat      MetalVertexFormatFromModelIO(MDL::VertexFormat vertexFormat);
} // MTK

namespace MTK
{
_MTK_PRIVATE_DEF_STR(NS::String*, ModelErrorDomain);
_MTK_PRIVATE_DEF_STR(NS::String*, ModelErrorKey);
} // MTK

_MTK_INLINE MTK::MeshBufferAllocator* MTK::MeshBufferAllocator::alloc()
{
    return NS::Object::alloc<MeshBufferAllocator>(_MTK_PRIVATE_CLS(MTKMeshBufferAllocator));
}

_MTK_INLINE MTK::MeshBufferAllocator* MTK::MeshBufferAllocator::initWithDevice(MTL::Device* pDevice)
{
    return Object::sendMessage<MeshBufferAllocator*>(this, _MTK_PRIVATE_SEL(initWithDevice_), pDevice);
}

_MTK_INLINE MTL::Device* MTK::MeshBufferAllocator::device() const
{
    return Object::sendMessage<MTL::Device*>(this, _MTK_PRIVATE_SEL(device));
}

_MTK_INLINE NS::UInteger MTK::MeshBuffer::length() const
{
    return Object::sendMessage<NS::UInteger>(this, _MTK_PRIVATE_SEL(length));
}

_MTK_INLINE MTK::MeshBufferAllocator* MTK::MeshBuffer::allocator() const
{
    return Object::sendMessage<MeshBufferAllocator*>(this, _MTK_PRIVATE_SEL(allocator));
}

_MTK_INLINE MDL::MeshBufferZone* MTK::MeshBuffer::zone() const
{
    return Object::sendMessage<MDL::MeshBufferZone*>(this, _MTK_PRIVATE_SEL(zone));
}

_MTK_INLINE MTL::Buffer* MTK::MeshBuffer::buffer() const
{
    return Object::sendMessage<MTL::Buffer*>(this, _MTK_PRIVATE_SEL(buffer));
}

_MTK_INLINE NS::UInteger MTK::MeshBuffer::offset() const
{
    return Object::sendMessage<NS::UInteger>(this, _MTK_PRIVATE_SEL(offset));
}

_MTK_INLINE MDL::MeshBufferType MTK::MeshBuffer::type() const
{
    return Object::sendMessage<MDL::MeshBufferType>(this, _MTK_PRIVATE_SEL(type));
}

_MTK_INLINE MTL::PrimitiveType MTK::Submesh::primitiveType() const
{
    return Object::sendMessage<MTL::PrimitiveType>(this, _MTK_PRIVATE_SEL(primitiveType));
}

_MTK_INLINE MTL::IndexType MTK::Submesh::indexType() const
{
    return Object::sendMessage<MTL::IndexType>(this, _MTK_PRIVATE_SEL(indexType));
}

_MTK_INLINE MTK::MeshBuffer* MTK::Submesh::indexBuffer() const
{
    return Object::sendMessage<MeshBuffer*>(this, _MTK_PRIVATE_SEL(indexBuffer));
}

_MTK_INLINE NS::UInteger MTK::Submesh::indexCount() const
{
    return Object::sendMessage<NS::UInteger>(this, _MTK_PRIVATE_SEL(indexCount));
}

_MTK_INLINE MTK::Mesh* MTK::Submesh::mesh() const
{
    return Object::sendMessage<Mesh*>(this, _MTK_PRIVATE_SEL(mesh));
}

_MTK_INLINE NS::String* MTK::Submesh::name() const
{
    return Object::sendMessage<NS::String*>(this, _MTK_PRIVATE_SEL(name));
}

_MTK_INLINE void MTK::Submesh::setName(const NS::String* pName)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setName_), pName);
}

_MTK_INLINE MTK::Mesh* MTK::Mesh::alloc()
{
    return NS::Object::alloc<Mesh>(_MTK_PRIVATE_CLS(MTKMesh));
}

_MTK_INLINE MTK::Mesh* MTK::Mesh::initWithMesh(MDL::Mesh* pMesh, MTL::Device* pDevice, NS::Error** ppError)
{
    return Object::sendMessage<Mesh*>(this, _MTK_PRIVATE_SEL(initWithMesh_device_error_), pMesh, pDevice, ppError);
}

_MTK_INLINE NS::Array* MTK::Mesh::newMeshesFromAsset(MDL::Asset* pAsset, MTL::Device* pDevice, NS::Array** ppSourceMeshes, NS::Error** ppError)
{
    return Object::sendMessage<NS::Array*>(_MTK_PRIVATE_CLS(MTKMesh), _MTK_PRIVATE_SEL(newMeshesFromAsset_device_sourceMeshes_error_), pAsset, pDevice, ppSourceMeshes, ppError);
}

_MTK_INLINE NS::Array* MTK::Mesh::vertexBuffers() const
{
    return Object::sendMessage<NS::Array*>(this, _MTK_PRIVATE_SEL(vertexBuffers));
}

_MTK_INLINE MDL::VertexDescriptor* MTK::Mesh::vertexDescriptor() const
{
    return Object::sendMessage<MDL::VertexDescriptor*>(this, _MTK_PRIVATE_SEL(vertexDescriptor));
}

_MTK_INLINE NS::Array* MTK::Mesh::submeshes() const
{
    return Object::sendMessage<NS::Array*>(this, _MTK_PRIVATE_SEL(submeshes));
}

_MTK_INLINE NS::UInteger MTK::Mesh::vertexCount() const
{
    return Object::sendMessage<NS::UInteger>(this, _MTK_PRIVATE_SEL(vertexCount));
}

_MTK_INLINE NS::String* MTK::Mesh::name() const
{
    return Object::sendMessage<NS::String*>(this, _MTK_PRIVATE_SEL(name));
}

_MTK_INLINE void MTK::Mesh::setName(const NS::String* pName)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setName_), pName);
}

_MTK_INLINE MDL::VertexDescriptor* MTK::ModelIOVertexDescriptorFromMetal(MTL::VertexDescriptor* pDescriptor)
{
    return static_cast<MDL::VertexDescriptor*>(MTKModelIOVertexDescriptorFromMetal(pDescriptor));
}

_MTK_INLINE MDL::VertexDescriptor* MTK::ModelIOVertexDescriptorFromMetalWithError(MTL::VertexDescriptor* pDescriptor, NS::Error** ppError)
{
    return static_cast<MDL::VertexDescriptor*>(MTKModelIOVertexDescriptorFromMetalWithError(pDescriptor, reinterpret_cast<void**>(ppError)));
}

_MTK_INLINE MTL::VertexDescriptor* MTK::MetalVertexDescriptorFromModelIO(MDL::VertexDescriptor* pDescriptor)
{
    return static_cast<MTL::VertexDescriptor*>(MTKMetalVertexDescriptorFromModelIO(pDescriptor));
}

_MTK_INLINE MTL::VertexDescriptor* MTK::MetalVertexDescriptorFromModelIOWithError(MDL::VertexDescriptor* pDescriptor, NS::Error** ppError)
{
    return static_cast<MTL::VertexDescriptor*>(MTKMetalVertexDescriptorFromModelIOWithError(pDescriptor, reinterpret_cast<void**>(ppError)));
}

_MTK_INLINE MDL::VertexFormat MTK::ModelIOVertexFormatFromMetal(MTL::VertexFormat vertexFormat)
{
    return static_cast<MDL::VertexFormat>(MTKModelIOVertexFormatFromMetal(static_cast<NS::UInteger>(vertexFormat)));
}

_MTK_INLINE MTL::VertexFormat MTK::MetalVertexFormatFromModelIO(MDL::VertexFormat vertexFormat)
{
    return static_cast<MTL::VertexFormat>(MTKMetalVertexFormatFromModelIO(static_cast<NS::UInteger>(vertexFormat)));
}
