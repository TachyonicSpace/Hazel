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

        virtual uint32_t GetColorAttachmentID() const override { return m_ColorAttachment; }

        virtual const FramebufferSpecs& GetSpecs() const override { return m_Specification; }
    private:
		uint32_t m_IDAttachment = 0;

		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
        FramebufferSpecs m_Specification;
    };

}