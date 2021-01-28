#pragma once

#include "Hazel/core/Core.h"
#include "glm/glm.hpp"


namespace Hazel
{

	enum class FramebufferTextureFormat
	{
		None = 0,

		//color
		RGBA8,
		RED,
		RED_INTEGER,

		//depth/stencil
		DEPTH24STENCIL8,

		//defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: m_TextureFormat(format) {}

		FramebufferTextureFormat m_TextureFormat = FramebufferTextureFormat::None;

		//todo: filtering/wrap
	};

	struct FrambufferAttatchmentSpecification
	{
		FrambufferAttatchmentSpecification() = default;
		FrambufferAttatchmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			:Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecs
	{
		uint32_t Width = -1;
		uint32_t Height = -1;
		FrambufferAttatchmentSpecification Attatchments;
		uint32_t samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		static Ref<Framebuffer> Create(const FramebufferSpecs& specs);


		virtual const FramebufferSpecs& GetSpecs() const = 0;
		virtual uint32_t GetColorAttachmentID(uint32_t index = 0) const = 0;

		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int Pixel(int buffer, int x, int y) = 0;
		virtual glm::vec4 Pixel4(int buffer, int x, int y) = 0;
	};

}