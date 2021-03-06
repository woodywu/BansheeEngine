//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "BsVulkanResource.h"

namespace bs
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/** Represents a single attachment in a Vulkan frame-buffer. */
	struct VULKAN_ATTACHMENT_DESC
	{
		/** Image to attach or null if none. */
		VulkanImage* image;

		/** View of the image to attach or VK_NULL_HANDLE if none. */
		VkImageView view;

		/** Format of the attached image. */
		VkFormat format;

		/** Initial layer of the surface as pointed to by the provided image view. */
		UINT32 baseLayer;
	};

	/** Contains parameters used for initializing VulkanFrameBuffer object. */
	struct VULKAN_FRAMEBUFFER_DESC
	{
		/** Images describing the color attachments. */
		VULKAN_ATTACHMENT_DESC color[BS_MAX_MULTIPLE_RENDER_TARGETS];

		/** Image describing the depth attachment. */
		VULKAN_ATTACHMENT_DESC depth;

		/** Width of the images, in pixels. All images must be the same size. */
		UINT32 width;

		/** Height of the images, in pixels. All images must be the same size. */
		UINT32 height;

		/** Number of image layers to render to. This value is used for all provided surfaces. */
		UINT32 layers;

		/** Number of samples in the attached images. All attachments must have the same number of samples. */
		UINT32 numSamples;

		/** Set to true if framebuffer represents an offscreen surface that will not be presented. */
		bool offscreen;
	};

	/** Information about a single framebuffer attachment. */
	struct VulkanFramebufferAttachment
	{
		VulkanImage* image = nullptr;
		UINT32 baseLayer = 0;
		VkImageLayout finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		UINT32 index = 0;
	};

	/** Vulkan frame buffer containing one or multiple color surfaces, and an optional depth surface. */
	class VulkanFramebuffer : public VulkanResource
	{
	public:
		/** Creates a new frame buffer with the specified image views attached. 
		 *
		 * @param[in]	owner	Resource manager that allocated this resource.
		 * @param[in]	desc	Description of the frame buffer.
		 */
		VulkanFramebuffer(VulkanResourceManager* owner, const VULKAN_FRAMEBUFFER_DESC& desc);
		~VulkanFramebuffer();

		/** Returns a unique ID of this framebuffer. */
		UINT32 getId() const { return mId; }

		/** 
		 * Gets internal Vulkan render pass object. 
		 * 
		 * @param[in]	loadMask	Mask that control which render target surface contents should be preserved on load.
		 * @param[in]	readMask	Mask that controls which render targets can be read by shaders while they're bound.
		 */
		VkRenderPass getRenderPass(RenderSurfaceMask loadMask, RenderSurfaceMask readMask) const;

		/**
		 * Gets internal Vulkan framebuffer object.
		 *
		 * @param[in]	loadMask	Mask that control which render target surface contents should be preserved on load.
		 * @param[in]	readMask	Mask that controls which render targets can be read by shaders while they're bound.
		 */
		VkFramebuffer getFramebuffer(RenderSurfaceMask loadMask, RenderSurfaceMask readMask) const;

		/** 
		 * Gets the number of layers in each framebuffer surface. A layer is an element in a texture array, or a depth 
		 * slice in a 3D texture).
		 */
		UINT32 getNumLayers() const { return mNumLayers; }

		/** Returns information about a color attachment at the specified index. */
		const VulkanFramebufferAttachment& getColorAttachment(UINT32 colorIdx) const { return mColorAttachments[colorIdx]; }

		/** Returns information about a depth-stencil attachment. */
		const VulkanFramebufferAttachment& getDepthStencilAttachment() const { return mDepthStencilAttachment; }

		/** Gets the total number of frame-buffer attachments, including both color and depth. */
		UINT32 getNumAttachments() const { return mNumAttachments; }

		/** Gets the number of color frame-buffer attachments. */
		UINT32 getNumColorAttachments() const { return mNumColorAttachments; }

		/** Returns true if the framebuffer has a depth attachment. */
		bool hasDepthAttachment() const { return mHasDepth; }

		/** Returns sample flags that determine if the framebuffer supports multi-sampling, and for how many samples. */
		VkSampleCountFlagBits getSampleFlags() const { return mSampleFlags; }
	private:
		/** Information about a single frame-buffer variant. */
		struct Variant
		{
			VkRenderPass renderPass;
			VkFramebuffer framebuffer;
		};

		/** Key used for identifying different types of frame-buffer variants. */
		struct VariantKey
		{
			VariantKey(RenderSurfaceMask loadMask, RenderSurfaceMask readMask);

			class HashFunction
			{
			public:
				size_t operator()(const VariantKey& key) const;
			};

			class EqualFunction
			{
			public:
				bool operator()(const VariantKey& lhs, const VariantKey& rhs) const;
			};

			RenderSurfaceMask loadMask;
			RenderSurfaceMask readMask;
		};

		/** Creates a new variant of the framebuffer. */
		Variant createVariant(RenderSurfaceMask loadMask, RenderSurfaceMask readMask) const;

		UINT32 mId;

		Variant mDefault;
		mutable UnorderedMap<VariantKey, Variant, VariantKey::HashFunction, VariantKey::EqualFunction> mVariants;

		UINT32 mNumAttachments;
		UINT32 mNumColorAttachments;
		UINT32 mNumLayers;
		VulkanFramebufferAttachment mColorAttachments[BS_MAX_MULTIPLE_RENDER_TARGETS];
		VulkanFramebufferAttachment mDepthStencilAttachment;
		bool mHasDepth;
		VkSampleCountFlagBits mSampleFlags;

		mutable VkAttachmentDescription mAttachments[BS_MAX_MULTIPLE_RENDER_TARGETS + 1];
		mutable VkImageView mAttachmentViews[BS_MAX_MULTIPLE_RENDER_TARGETS + 1];
		mutable VkAttachmentReference mColorReferences[BS_MAX_MULTIPLE_RENDER_TARGETS];
		mutable VkAttachmentReference mDepthReference;
		mutable VkSubpassDescription mSubpassDesc;
		mutable VkSubpassDependency mDependencies[2];
		mutable VkRenderPassCreateInfo mRenderPassCI;
		mutable VkFramebufferCreateInfo mFramebufferCI;

		static UINT32 sNextValidId;
	};

	/** @} */
}
