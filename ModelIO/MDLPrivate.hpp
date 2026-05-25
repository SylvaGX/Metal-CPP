//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// ModelIO/MDLPrivate.hpp
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

#include "MDLDefines.hpp"

#include <objc/runtime.h>

#define _MDL_PRIVATE_CLS(symbol) (MDL::Private::Class::s_k##symbol)
#define _MDL_PRIVATE_SEL(accessor) (MDL::Private::Selector::s_k##accessor)

#if defined(MDL_PRIVATE_IMPLEMENTATION)

#ifdef METALCPP_SYMBOL_VISIBILITY_HIDDEN
#define _MDL_PRIVATE_VISIBILITY __attribute__((visibility("hidden")))
#else
#define _MDL_PRIVATE_VISIBILITY __attribute__((visibility("default")))
#endif

#define _MDL_PRIVATE_IMPORT __attribute__((weak_import))

#ifdef __OBJC__
#define _MDL_PRIVATE_OBJC_LOOKUP_CLASS(symbol) ((__bridge void*)objc_lookUpClass(#symbol))
#define _MDL_PRIVATE_OBJC_GET_PROTOCOL(symbol) ((__bridge void*)objc_getProtocol(#symbol))
#else
#define _MDL_PRIVATE_OBJC_LOOKUP_CLASS(symbol) objc_lookUpClass(#symbol)
#define _MDL_PRIVATE_OBJC_GET_PROTOCOL(symbol) objc_getProtocol(#symbol)
#endif

#define _MDL_PRIVATE_DEF_CLS(symbol) void* s_k##symbol _MDL_PRIVATE_VISIBILITY = _MDL_PRIVATE_OBJC_LOOKUP_CLASS(symbol)
#define _MDL_PRIVATE_DEF_PRO(symbol) void* s_k##symbol _MDL_PRIVATE_VISIBILITY = _MDL_PRIVATE_OBJC_GET_PROTOCOL(symbol)
#define _MDL_PRIVATE_DEF_SEL(accessor, symbol) SEL s_k##accessor _MDL_PRIVATE_VISIBILITY = sel_registerName(symbol)

#define _MDL_PRIVATE_DEF_STR(type, symbol)                \
    _MDL_EXTERN type const MDL##symbol _MDL_PRIVATE_IMPORT; \
    type const             symbol = (nullptr != &MDL##symbol) ? MDL##symbol : nullptr

#else

#define _MDL_PRIVATE_DEF_CLS(symbol) extern void* s_k##symbol
#define _MDL_PRIVATE_DEF_PRO(symbol) extern void* s_k##symbol
#define _MDL_PRIVATE_DEF_SEL(accessor, symbol) extern SEL s_k##accessor
#define _MDL_PRIVATE_DEF_STR(type, symbol) extern type const MDL::symbol

#endif

namespace MDL::Private
{
namespace Class
{
    _MDL_PRIVATE_DEF_CLS(MDLAsset);
    _MDL_PRIVATE_DEF_CLS(MDLMesh);
    _MDL_PRIVATE_DEF_CLS(MDLTexture);
    _MDL_PRIVATE_DEF_CLS(MDLVertexDescriptor);
} // Class

namespace Protocol
{
    _MDL_PRIVATE_DEF_PRO(MDLMeshBufferZone);
} // Protocol

namespace Selector
{
    _MDL_PRIVATE_DEF_SEL(alloc, "alloc");
    _MDL_PRIVATE_DEF_SEL(init, "init");
    _MDL_PRIVATE_DEF_SEL(initWithURL_, "initWithURL:");
    _MDL_PRIVATE_DEF_SEL(initWithURL_vertexDescriptor_bufferAllocator_, "initWithURL:vertexDescriptor:bufferAllocator:");
} // Selector
} // MDL::Private
