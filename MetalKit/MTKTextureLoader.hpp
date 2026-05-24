//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// MetalKit/MTKTextureLoader.hpp
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

#include "../AppKit/AKAppKitEnums.hpp"
#include "../Foundation/Foundation.hpp"
#include "../Metal/MTLTexture.hpp"
#include "MTKDefines.hpp"
#include "MTKPrivate.hpp"
#include "MTKTypes.hpp"

#include <CoreGraphics/CGGeometry.h>
#include <CoreGraphics/CGImage.h>
#include <functional>

namespace MTK
{
using TextureLoaderError = NS::String*;
using TextureLoaderOption = NS::String*;
using TextureLoaderCubeLayout = NS::String*;
using TextureLoaderOrigin = NS::String*;

using TextureLoaderCallback = void (^)(MTL::Texture* pTexture, NS::Error* pError);
using TextureLoaderArrayCallback = void (^)(NS::Array* pTextures, NS::Error* pError);
using TextureLoaderCallbackFunction = std::function<void(MTL::Texture* pTexture, NS::Error* pError)>;
using TextureLoaderArrayCallbackFunction = std::function<void(NS::Array* pTextures, NS::Error* pError)>;

_MTK_CONST(TextureLoaderError, TextureLoaderErrorDomain);
_MTK_CONST(TextureLoaderError, TextureLoaderErrorKey);
_MTK_CONST(TextureLoaderOption, TextureLoaderOptionAllocateMipmaps);
_MTK_CONST(TextureLoaderOption, TextureLoaderOptionGenerateMipmaps);
_MTK_CONST(TextureLoaderOption, TextureLoaderOptionSRGB);
_MTK_CONST(TextureLoaderOption, TextureLoaderOptionTextureUsage);
_MTK_CONST(TextureLoaderOption, TextureLoaderOptionTextureCPUCacheMode);
_MTK_CONST(TextureLoaderOption, TextureLoaderOptionTextureStorageMode);
_MTK_CONST(TextureLoaderOption, TextureLoaderOptionCubeLayout);
_MTK_CONST(TextureLoaderOption, TextureLoaderOptionOrigin);
_MTK_CONST(TextureLoaderOption, TextureLoaderOptionLoadAsArray);
_MTK_CONST(TextureLoaderCubeLayout, TextureLoaderCubeLayoutVertical);
_MTK_CONST(TextureLoaderOrigin, TextureLoaderOriginTopLeft);
_MTK_CONST(TextureLoaderOrigin, TextureLoaderOriginBottomLeft);
_MTK_CONST(TextureLoaderOrigin, TextureLoaderOriginFlippedVertically);

class TextureLoader : public NS::Referencing<TextureLoader>
{
public:
    static TextureLoader* alloc();

    TextureLoader* initWithDevice(MTL::Device* pDevice);

    MTL::Device* device() const;

    MTL::Texture* newTextureWithContentsOfURL(const NS::URL* pURL, const NS::Dictionary* pOptions, NS::Error** ppError);
    NS::Array*    newTexturesWithContentsOfURLs(const NS::Array* pURLs, const NS::Dictionary* pOptions, NS::Error** ppError);
    MTL::Texture* newTextureWithData(const NS::Data* pData, const NS::Dictionary* pOptions, NS::Error** ppError);
    MTL::Texture* newTextureWithCGImage(CGImageRef cgImage, const NS::Dictionary* pOptions, NS::Error** ppError);
    MTL::Texture* newTextureWithMDLTexture(MDL::Texture* pTexture, const NS::Dictionary* pOptions, NS::Error** ppError);
    MTL::Texture* newTextureWithName(const NS::String* pName, CGFloat scaleFactor, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, NS::Error** ppError);
    MTL::Texture* newTextureWithName(const NS::String* pName, CGFloat scaleFactor, NS::DisplayGamut displayGamut, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, NS::Error** ppError);

    void newTextureWithContentsOfURL(const NS::URL* pURL, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler);
    void newTexturesWithContentsOfURLs(const NS::Array* pURLs, const NS::Dictionary* pOptions, const TextureLoaderArrayCallback completionHandler);
    void newTextureWithData(const NS::Data* pData, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler);
    void newTextureWithCGImage(CGImageRef cgImage, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler);
    void newTextureWithMDLTexture(MDL::Texture* pTexture, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler);
    void newTextureWithName(const NS::String* pName, CGFloat scaleFactor, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler);
    void newTextureWithName(const NS::String* pName, CGFloat scaleFactor, NS::DisplayGamut displayGamut, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler);
    void newTexturesWithNames(const NS::Array* pNames, CGFloat scaleFactor, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderArrayCallback completionHandler);
    void newTexturesWithNames(const NS::Array* pNames, CGFloat scaleFactor, NS::DisplayGamut displayGamut, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderArrayCallback completionHandler);

    void newTextureWithContentsOfURL(const NS::URL* pURL, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler);
    void newTexturesWithContentsOfURLs(const NS::Array* pURLs, const NS::Dictionary* pOptions, const TextureLoaderArrayCallbackFunction& completionHandler);
    void newTextureWithData(const NS::Data* pData, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler);
    void newTextureWithCGImage(CGImageRef cgImage, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler);
    void newTextureWithMDLTexture(MDL::Texture* pTexture, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler);
    void newTextureWithName(const NS::String* pName, CGFloat scaleFactor, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler);
    void newTextureWithName(const NS::String* pName, CGFloat scaleFactor, NS::DisplayGamut displayGamut, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler);
    void newTexturesWithNames(const NS::Array* pNames, CGFloat scaleFactor, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderArrayCallbackFunction& completionHandler);
    void newTexturesWithNames(const NS::Array* pNames, CGFloat scaleFactor, NS::DisplayGamut displayGamut, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderArrayCallbackFunction& completionHandler);
};
} // MTK

namespace MTK
{
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderErrorDomain);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderErrorKey);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOptionAllocateMipmaps);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOptionGenerateMipmaps);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOptionSRGB);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOptionTextureUsage);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOptionTextureCPUCacheMode);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOptionTextureStorageMode);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOptionCubeLayout);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOptionOrigin);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOptionLoadAsArray);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderCubeLayoutVertical);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOriginTopLeft);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOriginBottomLeft);
_MTK_PRIVATE_DEF_STR(NS::String*, TextureLoaderOriginFlippedVertically);
} // MTK

_MTK_INLINE MTK::TextureLoader* MTK::TextureLoader::alloc()
{
    return NS::Object::alloc<TextureLoader>(_MTK_PRIVATE_CLS(MTKTextureLoader));
}

_MTK_INLINE MTK::TextureLoader* MTK::TextureLoader::initWithDevice(MTL::Device* pDevice)
{
    return Object::sendMessage<TextureLoader*>(this, _MTK_PRIVATE_SEL(initWithDevice_), pDevice);
}

_MTK_INLINE MTL::Device* MTK::TextureLoader::device() const
{
    return Object::sendMessage<MTL::Device*>(this, _MTK_PRIVATE_SEL(device));
}

_MTK_INLINE MTL::Texture* MTK::TextureLoader::newTextureWithContentsOfURL(const NS::URL* pURL, const NS::Dictionary* pOptions, NS::Error** ppError)
{
    return Object::sendMessage<MTL::Texture*>(this, _MTK_PRIVATE_SEL(newTextureWithContentsOfURL_options_error_), pURL, pOptions, ppError);
}

_MTK_INLINE NS::Array* MTK::TextureLoader::newTexturesWithContentsOfURLs(const NS::Array* pURLs, const NS::Dictionary* pOptions, NS::Error** ppError)
{
    return Object::sendMessage<NS::Array*>(this, _MTK_PRIVATE_SEL(newTexturesWithContentsOfURLs_options_error_), pURLs, pOptions, ppError);
}

_MTK_INLINE MTL::Texture* MTK::TextureLoader::newTextureWithData(const NS::Data* pData, const NS::Dictionary* pOptions, NS::Error** ppError)
{
    return Object::sendMessage<MTL::Texture*>(this, _MTK_PRIVATE_SEL(newTextureWithData_options_error_), pData, pOptions, ppError);
}

_MTK_INLINE MTL::Texture* MTK::TextureLoader::newTextureWithCGImage(CGImageRef cgImage, const NS::Dictionary* pOptions, NS::Error** ppError)
{
    return Object::sendMessage<MTL::Texture*>(this, _MTK_PRIVATE_SEL(newTextureWithCGImage_options_error_), cgImage, pOptions, ppError);
}

_MTK_INLINE MTL::Texture* MTK::TextureLoader::newTextureWithMDLTexture(MDL::Texture* pTexture, const NS::Dictionary* pOptions, NS::Error** ppError)
{
    return Object::sendMessage<MTL::Texture*>(this, _MTK_PRIVATE_SEL(newTextureWithMDLTexture_options_error_), pTexture, pOptions, ppError);
}

_MTK_INLINE MTL::Texture* MTK::TextureLoader::newTextureWithName(const NS::String* pName, CGFloat scaleFactor, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, NS::Error** ppError)
{
    return Object::sendMessage<MTL::Texture*>(this, _MTK_PRIVATE_SEL(newTextureWithName_scaleFactor_bundle_options_error_), pName, scaleFactor, pBundle, pOptions, ppError);
}

_MTK_INLINE MTL::Texture* MTK::TextureLoader::newTextureWithName(const NS::String* pName, CGFloat scaleFactor, NS::DisplayGamut displayGamut, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, NS::Error** ppError)
{
    return Object::sendMessage<MTL::Texture*>(this, _MTK_PRIVATE_SEL(newTextureWithName_scaleFactor_displayGamut_bundle_options_error_), pName, scaleFactor, displayGamut, pBundle, pOptions, ppError);
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithContentsOfURL(const NS::URL* pURL, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(newTextureWithContentsOfURL_options_completionHandler_), pURL, pOptions, completionHandler);
}

_MTK_INLINE void MTK::TextureLoader::newTexturesWithContentsOfURLs(const NS::Array* pURLs, const NS::Dictionary* pOptions, const TextureLoaderArrayCallback completionHandler)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(newTexturesWithContentsOfURLs_options_completionHandler_), pURLs, pOptions, completionHandler);
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithData(const NS::Data* pData, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(newTextureWithData_options_completionHandler_), pData, pOptions, completionHandler);
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithCGImage(CGImageRef cgImage, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(newTextureWithCGImage_options_completionHandler_), cgImage, pOptions, completionHandler);
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithMDLTexture(MDL::Texture* pTexture, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(newTextureWithMDLTexture_options_completionHandler_), pTexture, pOptions, completionHandler);
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithName(const NS::String* pName, CGFloat scaleFactor, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(newTextureWithName_scaleFactor_bundle_options_completionHandler_), pName, scaleFactor, pBundle, pOptions, completionHandler);
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithName(const NS::String* pName, CGFloat scaleFactor, NS::DisplayGamut displayGamut, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderCallback completionHandler)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(newTextureWithName_scaleFactor_displayGamut_bundle_options_completionHandler_), pName, scaleFactor, displayGamut, pBundle, pOptions, completionHandler);
}

_MTK_INLINE void MTK::TextureLoader::newTexturesWithNames(const NS::Array* pNames, CGFloat scaleFactor, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderArrayCallback completionHandler)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(newTexturesWithNames_scaleFactor_bundle_options_completionHandler_), pNames, scaleFactor, pBundle, pOptions, completionHandler);
}

_MTK_INLINE void MTK::TextureLoader::newTexturesWithNames(const NS::Array* pNames, CGFloat scaleFactor, NS::DisplayGamut displayGamut, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderArrayCallback completionHandler)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(newTexturesWithNames_scaleFactor_displayGamut_bundle_options_completionHandler_), pNames, scaleFactor, displayGamut, pBundle, pOptions, completionHandler);
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithContentsOfURL(const NS::URL* pURL, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler)
{
    __block TextureLoaderCallbackFunction function = completionHandler;
    newTextureWithContentsOfURL(pURL, pOptions, ^(MTL::Texture* pTexture, NS::Error* pError) { function(pTexture, pError); });
}

_MTK_INLINE void MTK::TextureLoader::newTexturesWithContentsOfURLs(const NS::Array* pURLs, const NS::Dictionary* pOptions, const TextureLoaderArrayCallbackFunction& completionHandler)
{
    __block TextureLoaderArrayCallbackFunction function = completionHandler;
    newTexturesWithContentsOfURLs(pURLs, pOptions, ^(NS::Array* pTextures, NS::Error* pError) { function(pTextures, pError); });
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithData(const NS::Data* pData, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler)
{
    __block TextureLoaderCallbackFunction function = completionHandler;
    newTextureWithData(pData, pOptions, ^(MTL::Texture* pTexture, NS::Error* pError) { function(pTexture, pError); });
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithCGImage(CGImageRef cgImage, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler)
{
    __block TextureLoaderCallbackFunction function = completionHandler;
    newTextureWithCGImage(cgImage, pOptions, ^(MTL::Texture* pTexture, NS::Error* pError) { function(pTexture, pError); });
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithMDLTexture(MDL::Texture* pTexture, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler)
{
    __block TextureLoaderCallbackFunction function = completionHandler;
    newTextureWithMDLTexture(pTexture, pOptions, ^(MTL::Texture* pTextureOut, NS::Error* pError) { function(pTextureOut, pError); });
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithName(const NS::String* pName, CGFloat scaleFactor, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler)
{
    __block TextureLoaderCallbackFunction function = completionHandler;
    newTextureWithName(pName, scaleFactor, pBundle, pOptions, ^(MTL::Texture* pTexture, NS::Error* pError) { function(pTexture, pError); });
}

_MTK_INLINE void MTK::TextureLoader::newTextureWithName(const NS::String* pName, CGFloat scaleFactor, NS::DisplayGamut displayGamut, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderCallbackFunction& completionHandler)
{
    __block TextureLoaderCallbackFunction function = completionHandler;
    newTextureWithName(pName, scaleFactor, displayGamut, pBundle, pOptions, ^(MTL::Texture* pTexture, NS::Error* pError) { function(pTexture, pError); });
}

_MTK_INLINE void MTK::TextureLoader::newTexturesWithNames(const NS::Array* pNames, CGFloat scaleFactor, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderArrayCallbackFunction& completionHandler)
{
    __block TextureLoaderArrayCallbackFunction function = completionHandler;
    newTexturesWithNames(pNames, scaleFactor, pBundle, pOptions, ^(NS::Array* pTextures, NS::Error* pError) { function(pTextures, pError); });
}

_MTK_INLINE void MTK::TextureLoader::newTexturesWithNames(const NS::Array* pNames, CGFloat scaleFactor, NS::DisplayGamut displayGamut, const NS::Bundle* pBundle, const NS::Dictionary* pOptions, const TextureLoaderArrayCallbackFunction& completionHandler)
{
    __block TextureLoaderArrayCallbackFunction function = completionHandler;
    newTexturesWithNames(pNames, scaleFactor, displayGamut, pBundle, pOptions, ^(NS::Array* pTextures, NS::Error* pError) { function(pTextures, pError); });
}
