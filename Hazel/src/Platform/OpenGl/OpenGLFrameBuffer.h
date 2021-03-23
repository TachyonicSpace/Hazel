#pragma once

#include "Hazel/Renderer/Framebuffer.h"
#include "glad/glad.h"

namespace Hazel
{

    class OpenGLFrameBuffer : public Framebuffer
    {
    public:
        OpenGLFrameBuffer(const FramebufferSpecs& specs);
        virtual ~OpenGLFrameBuffer();

        void Invalidate();

        virtual void Bind() override;
        virtual void UnBind() override;

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual uint32_t GetColorAttachmentID(uint32_t index = 0) const override { HZ_CORE_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }

        virtual const FramebufferSpecs& GetSpecs() const override { return m_Specification; }
    private:
		uint32_t m_IDAttachment = 0;

		uint32_t m_RendererID = 0;
        FramebufferSpecs m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
        FramebufferTextureSpecification m_DepthAttachmentSpec = FramebufferTextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;

		virtual float Pixel(int buffer, int x, int y);
		virtual glm::vec4 Pixel4(int buffer, int x, int y);
    };

}