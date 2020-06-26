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

		virtual void Bind() override {   glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);}
		virtual void UnBind() override {	glBindFramebuffer(GL_FRAMEBUFFER, 0);}

        void Invalidate();

        virtual uint32_t GetColorAttachmentID() const override { return m_ColorAttachment; }

    private:
        uint32_t m_RendererID;
        uint32_t m_ColorAttachment;
        uint32_t m_DepthAttachment;
        FramebufferSpecs m_Specs;
    };

}