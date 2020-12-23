#pragma once

#include "Hazel/Renderer/Framebuffer.h"
#include "glad/glad.h"

namespace Hazel
{

    class OpenGLFrameBuffer :
        public Framebuffer
    {
    public:
        OpenGLFrameBuffer(const FramebufferSpecs& specs);
        virtual ~OpenGLFrameBuffer();


        virtual const FramebufferSpecs& GetSpecs() const override { return m_Specs; }

        virtual void Bind() override;
		virtual void UnBind() override {	glBindFramebuffer(GL_FRAMEBUFFER, 0);}

        virtual void Resize(uint32_t width, uint32_t height) override;

        void Invalidate();

        virtual uint32_t GetColorAttachmentID() const override { return m_ColorAttachment; }

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_ColorAttachment = 0;
		uint32_t m_IDAttachment = 0;
		uint32_t m_DepthAttachment = 0;
        FramebufferSpecs m_Specs;
    };

}