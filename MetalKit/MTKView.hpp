//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// MetalKit/MTKView.hpp
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

#include "../AppKit/Window/NSView.hpp"
#include "../Metal/MTLDevice.hpp"
#include "../Metal/MTLPixelFormat.hpp"
#include "../Metal/MTLRenderPass.hpp"
#include "../Metal/MTLResource.hpp"
#include "../Metal/MTLTexture.hpp"
#include "../QuartzCore/CAMetalDrawable.hpp"
#include "MTKDefines.hpp"
#include "MTKPrivate.hpp"

#include <CoreGraphics/CGColorSpace.h>

namespace MTK
{
class View : public NS::View
{
public:
    static View* alloc();

    View* initWithFrame(NS::Rect frame, MTL::Device* pDevice);
    View* initWithCoder(const NS::Coder* pCoder);

    void* delegate() const;
    void  setDelegate(void* pDelegate);

    MTL::Device* device() const;
    void         setDevice(MTL::Device* pDevice);

    CA::MetalDrawable* currentDrawable() const;

    bool framebufferOnly() const;
    void setFramebufferOnly(bool framebufferOnly);

    MTL::TextureUsage depthStencilAttachmentTextureUsage() const;
    void              setDepthStencilAttachmentTextureUsage(MTL::TextureUsage usage);

    MTL::TextureUsage multisampleColorAttachmentTextureUsage() const;
    void              setMultisampleColorAttachmentTextureUsage(MTL::TextureUsage usage);

    bool presentsWithTransaction() const;
    void setPresentsWithTransaction(bool presentsWithTransaction);

    MTL::PixelFormat colorPixelFormat() const;
    void             setColorPixelFormat(MTL::PixelFormat pixelFormat);

    MTL::PixelFormat depthStencilPixelFormat() const;
    void             setDepthStencilPixelFormat(MTL::PixelFormat pixelFormat);

    MTL::StorageMode depthStencilStorageMode() const;
    void             setDepthStencilStorageMode(MTL::StorageMode storageMode);

    NS::UInteger sampleCount() const;
    void         setSampleCount(NS::UInteger sampleCount);

    MTL::ClearColor clearColor() const;
    void            setClearColor(MTL::ClearColor clearColor);

    double   clearDepth() const;
    void     setClearDepth(double clearDepth);

    uint32_t clearStencil() const;
    void     setClearStencil(uint32_t clearStencil);

    MTL::Texture* depthStencilTexture() const;
    MTL::Texture* multisampleColorTexture() const;

    void releaseDrawables();

    MTL::RenderPassDescriptor* currentRenderPassDescriptor() const;

    NS::Integer preferredFramesPerSecond() const;
    void        setPreferredFramesPerSecond(NS::Integer preferredFramesPerSecond);

    bool enableSetNeedsDisplay() const;
    void setEnableSetNeedsDisplay(bool enableSetNeedsDisplay);

    bool autoResizeDrawable() const;
    void setAutoResizeDrawable(bool autoResizeDrawable);

    NS::Size drawableSize() const;
    void     setDrawableSize(NS::Size drawableSize);

    NS::Size preferredDrawableSize() const;

    MTL::Device* preferredDevice() const;

    bool isPaused() const;
    void setPaused(bool paused);

    CGColorSpaceRef colorspace() const;
    void            setColorspace(CGColorSpaceRef colorspace);

    void draw();
};
} // MTK

_MTK_INLINE MTK::View* MTK::View::alloc()
{
    return NS::Object::alloc<View>(_MTK_PRIVATE_CLS(MTKView));
}

_MTK_INLINE MTK::View* MTK::View::initWithFrame(NS::Rect frame, MTL::Device* pDevice)
{
    return Object::sendMessage<View*>(this, _MTK_PRIVATE_SEL(initWithFrame_device_), frame, pDevice);
}

_MTK_INLINE MTK::View* MTK::View::initWithCoder(const NS::Coder* pCoder)
{
    return Object::sendMessage<View*>(this, _MTK_PRIVATE_SEL(initWithCoder_), pCoder);
}

_MTK_INLINE void* MTK::View::delegate() const
{
    return Object::sendMessage<void*>(this, _MTK_PRIVATE_SEL(delegate));
}

_MTK_INLINE void MTK::View::setDelegate(void* pDelegate)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setDelegate_), pDelegate);
}

_MTK_INLINE MTL::Device* MTK::View::device() const
{
    return Object::sendMessage<MTL::Device*>(this, _MTK_PRIVATE_SEL(device));
}

_MTK_INLINE void MTK::View::setDevice(MTL::Device* pDevice)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setDevice_), pDevice);
}

_MTK_INLINE CA::MetalDrawable* MTK::View::currentDrawable() const
{
    return Object::sendMessage<CA::MetalDrawable*>(this, _MTK_PRIVATE_SEL(currentDrawable));
}

_MTK_INLINE bool MTK::View::framebufferOnly() const
{
    return Object::sendMessage<bool>(this, _MTK_PRIVATE_SEL(framebufferOnly));
}

_MTK_INLINE void MTK::View::setFramebufferOnly(bool framebufferOnly)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setFramebufferOnly_), framebufferOnly);
}

_MTK_INLINE MTL::TextureUsage MTK::View::depthStencilAttachmentTextureUsage() const
{
    return Object::sendMessage<MTL::TextureUsage>(this, _MTK_PRIVATE_SEL(depthStencilAttachmentTextureUsage));
}

_MTK_INLINE void MTK::View::setDepthStencilAttachmentTextureUsage(MTL::TextureUsage usage)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setDepthStencilAttachmentTextureUsage_), usage);
}

_MTK_INLINE MTL::TextureUsage MTK::View::multisampleColorAttachmentTextureUsage() const
{
    return Object::sendMessage<MTL::TextureUsage>(this, _MTK_PRIVATE_SEL(multisampleColorAttachmentTextureUsage));
}

_MTK_INLINE void MTK::View::setMultisampleColorAttachmentTextureUsage(MTL::TextureUsage usage)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setMultisampleColorAttachmentTextureUsage_), usage);
}

_MTK_INLINE bool MTK::View::presentsWithTransaction() const
{
    return Object::sendMessage<bool>(this, _MTK_PRIVATE_SEL(presentsWithTransaction));
}

_MTK_INLINE void MTK::View::setPresentsWithTransaction(bool presentsWithTransaction)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setPresentsWithTransaction_), presentsWithTransaction);
}

_MTK_INLINE MTL::PixelFormat MTK::View::colorPixelFormat() const
{
    return Object::sendMessage<MTL::PixelFormat>(this, _MTK_PRIVATE_SEL(colorPixelFormat));
}

_MTK_INLINE void MTK::View::setColorPixelFormat(MTL::PixelFormat pixelFormat)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setColorPixelFormat_), pixelFormat);
}

_MTK_INLINE MTL::PixelFormat MTK::View::depthStencilPixelFormat() const
{
    return Object::sendMessage<MTL::PixelFormat>(this, _MTK_PRIVATE_SEL(depthStencilPixelFormat));
}

_MTK_INLINE void MTK::View::setDepthStencilPixelFormat(MTL::PixelFormat pixelFormat)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setDepthStencilPixelFormat_), pixelFormat);
}

_MTK_INLINE MTL::StorageMode MTK::View::depthStencilStorageMode() const
{
    return Object::sendMessage<MTL::StorageMode>(this, _MTK_PRIVATE_SEL(depthStencilStorageMode));
}

_MTK_INLINE void MTK::View::setDepthStencilStorageMode(MTL::StorageMode storageMode)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setDepthStencilStorageMode_), storageMode);
}

_MTK_INLINE NS::UInteger MTK::View::sampleCount() const
{
    return Object::sendMessage<NS::UInteger>(this, _MTK_PRIVATE_SEL(sampleCount));
}

_MTK_INLINE void MTK::View::setSampleCount(NS::UInteger sampleCount)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setSampleCount_), sampleCount);
}

_MTK_INLINE MTL::ClearColor MTK::View::clearColor() const
{
    return Object::sendMessage<MTL::ClearColor>(this, _MTK_PRIVATE_SEL(clearColor));
}

_MTK_INLINE void MTK::View::setClearColor(MTL::ClearColor clearColor)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setClearColor_), clearColor);
}

_MTK_INLINE double MTK::View::clearDepth() const
{
    return Object::sendMessage<double>(this, _MTK_PRIVATE_SEL(clearDepth));
}

_MTK_INLINE void MTK::View::setClearDepth(double clearDepth)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setClearDepth_), clearDepth);
}

_MTK_INLINE uint32_t MTK::View::clearStencil() const
{
    return Object::sendMessage<uint32_t>(this, _MTK_PRIVATE_SEL(clearStencil));
}

_MTK_INLINE void MTK::View::setClearStencil(uint32_t clearStencil)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setClearStencil_), clearStencil);
}

_MTK_INLINE MTL::Texture* MTK::View::depthStencilTexture() const
{
    return Object::sendMessage<MTL::Texture*>(this, _MTK_PRIVATE_SEL(depthStencilTexture));
}

_MTK_INLINE MTL::Texture* MTK::View::multisampleColorTexture() const
{
    return Object::sendMessage<MTL::Texture*>(this, _MTK_PRIVATE_SEL(multisampleColorTexture));
}

_MTK_INLINE void MTK::View::releaseDrawables()
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(releaseDrawables));
}

_MTK_INLINE MTL::RenderPassDescriptor* MTK::View::currentRenderPassDescriptor() const
{
    return Object::sendMessage<MTL::RenderPassDescriptor*>(this, _MTK_PRIVATE_SEL(currentRenderPassDescriptor));
}

_MTK_INLINE NS::Integer MTK::View::preferredFramesPerSecond() const
{
    return Object::sendMessage<NS::Integer>(this, _MTK_PRIVATE_SEL(preferredFramesPerSecond));
}

_MTK_INLINE void MTK::View::setPreferredFramesPerSecond(NS::Integer preferredFramesPerSecond)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setPreferredFramesPerSecond_), preferredFramesPerSecond);
}

_MTK_INLINE bool MTK::View::enableSetNeedsDisplay() const
{
    return Object::sendMessage<bool>(this, _MTK_PRIVATE_SEL(enableSetNeedsDisplay));
}

_MTK_INLINE void MTK::View::setEnableSetNeedsDisplay(bool enableSetNeedsDisplay)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setEnableSetNeedsDisplay_), enableSetNeedsDisplay);
}

_MTK_INLINE bool MTK::View::autoResizeDrawable() const
{
    return Object::sendMessage<bool>(this, _MTK_PRIVATE_SEL(autoResizeDrawable));
}

_MTK_INLINE void MTK::View::setAutoResizeDrawable(bool autoResizeDrawable)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setAutoResizeDrawable_), autoResizeDrawable);
}

_MTK_INLINE NS::Size MTK::View::drawableSize() const
{
    return Object::sendMessage<NS::Size>(this, _MTK_PRIVATE_SEL(drawableSize));
}

_MTK_INLINE void MTK::View::setDrawableSize(NS::Size drawableSize)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setDrawableSize_), drawableSize);
}

_MTK_INLINE NS::Size MTK::View::preferredDrawableSize() const
{
    return Object::sendMessage<NS::Size>(this, _MTK_PRIVATE_SEL(preferredDrawableSize));
}

_MTK_INLINE MTL::Device* MTK::View::preferredDevice() const
{
    return Object::sendMessage<MTL::Device*>(this, _MTK_PRIVATE_SEL(preferredDevice));
}

_MTK_INLINE bool MTK::View::isPaused() const
{
    return Object::sendMessage<bool>(this, _MTK_PRIVATE_SEL(isPaused));
}

_MTK_INLINE void MTK::View::setPaused(bool paused)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setPaused_), paused);
}

_MTK_INLINE CGColorSpaceRef MTK::View::colorspace() const
{
    return Object::sendMessage<CGColorSpaceRef>(this, _MTK_PRIVATE_SEL(colorspace));
}

_MTK_INLINE void MTK::View::setColorspace(CGColorSpaceRef colorspace)
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(setColorspace_), colorspace);
}

_MTK_INLINE void MTK::View::draw()
{
    Object::sendMessage<void>(this, _MTK_PRIVATE_SEL(draw));
}
