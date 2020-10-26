#pragma once

#include "Hazel/core/Core.h"

namespace Hazel
{
	struct FramebufferSpecs
	{
		uint32_t Width = -1;
		uint32_t Height = -1;
		uint32_t samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		static Ref<Framebuffer> Create(const FramebufferSpecs& specs);


		virtual const FramebufferSpecs& GetSpecs() const = 0;
		virtual uint32_t GetColorAttachmentID() const = 0;

		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
	};

}