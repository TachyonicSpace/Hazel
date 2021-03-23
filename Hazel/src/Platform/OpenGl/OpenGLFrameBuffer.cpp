#include "hzpch.h"
#include "OpenGLFrameBuffer.h"

namespace Hazel
{

	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils
	{
		static GLenum TextureTarget(bool multiSamp)
		{
			return multiSamp ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				return true;
			default:
				return false;
			}
		}

		static void CreateTextures(bool multiSamples, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multiSamples), count, outID);
		}

		static void BindTexture(bool multisamp, uint32_t id)
		{
			glBindTexture(TextureTarget(multisamp), id);
		}

		static void AttachColorTexture(uint32_t id, uint32_t samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			if (samples > 1)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(samples > 1), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, uint32_t samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			if (samples > 1)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(samples > 1), id, 0);
		}
	}
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpecs& specs)
		:m_Specification(specs)
	{
		for (auto& spec : specs.Attatchments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.m_TextureFormat))
				m_ColorAttachmentSpecs.emplace_back(spec);
			else
				m_DepthAttachmentSpec = spec;
		}


		Invalidate();
	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}
	void OpenGLFrameBuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		//attachments
		bool multiSample = m_Specification.samples > 1;
		if (m_ColorAttachmentSpecs.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
			Utils::CreateTextures(multiSample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (int i = 0; i < m_ColorAttachments.size(); i++)
			{
				Utils::BindTexture(multiSample, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecs[i].m_TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::RED:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.samples, GL_R, GL_RED, m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::RED_INTEGER:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
					break;
				default:
					break;
				}
			}
		}

		if (m_DepthAttachmentSpec.m_TextureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multiSample, &m_DepthAttachment, 1);
			Utils::BindTexture(multiSample, m_DepthAttachment);

			switch (m_DepthAttachmentSpec.m_TextureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			default:
				break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			constexpr size_t maxSize = 4;
			//supports up to 31 attachments
			HZ_CORE_ASSERT(m_ColorAttachments.size() <= maxSize);
			GLenum buffers[maxSize] = {
				GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };/*, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
				GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
				GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15, GL_COLOR_ATTACHMENT16, GL_COLOR_ATTACHMENT17,
				GL_COLOR_ATTACHMENT18, GL_COLOR_ATTACHMENT19, GL_COLOR_ATTACHMENT20, GL_COLOR_ATTACHMENT21, GL_COLOR_ATTACHMENT22, GL_COLOR_ATTACHMENT23,
				GL_COLOR_ATTACHMENT24, GL_COLOR_ATTACHMENT25, GL_COLOR_ATTACHMENT26, GL_COLOR_ATTACHMENT27, GL_COLOR_ATTACHMENT28, GL_COLOR_ATTACHMENT29,
				GL_COLOR_ATTACHMENT30, GL_COLOR_ATTACHMENT31
			};*/
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		//only depth pass
		else if (m_ColorAttachments.empty())
			glDrawBuffer(GL_NONE);


		{
			//	//color attachment
			//	glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
			//	glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
			//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

			//	////ID buffer attachment
			//	//glCreateTextures(GL_TEXTURE_2D, 1, &m_IDAttachment);
			//	//glBindTexture(GL_TEXTURE_2D, m_IDAttachment);
			//	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, m_Specification.Width, m_Specification.Height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
			//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			//	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_IDAttachment, 0);

			//	//GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			//	//glDrawBuffers(2, drawBuffers);

			//	//depth attachment
			//	glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
			//	glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
			//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
			//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
		}

		HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);

		int clearValue = -1;
		glClearTexImage(m_ColorAttachments[1], 0, GL_RED_INTEGER, GL_INT, &clearValue);
	}
	void OpenGLFrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			HZ_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	};

	float OpenGLFrameBuffer::Pixel(int buffer, int x, int y)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + buffer);
		int i = -1;
		float j = -1;
		auto specs = m_ColorAttachmentSpecs[buffer].m_TextureFormat;
		//add switch
		switch (specs)
		{
		case Hazel::FramebufferTextureFormat::None:
			HZ_CORE_WARN("error, tried to read from a non-color buffer");
			break;
		case Hazel::FramebufferTextureFormat::RGBA8:
			glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &j);
			HZ_CORE_WARN("error, tried to read from a multi-channel color buffer");
			break;
		case Hazel::FramebufferTextureFormat::RED:
			glReadPixels(x, y, 1, 1, GL_RED, GL_FLOAT, &j);
			break;
		case Hazel::FramebufferTextureFormat::RED_INTEGER:
			glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &i);
			j = i;
			break;
		case Hazel::FramebufferTextureFormat::DEPTH24STENCIL8:
			HZ_CORE_WARN("error, tried to read from a non-color buffer");
			break;
		default:
			break;
		}

		return j;
	}

	glm::vec4 OpenGLFrameBuffer::Pixel4(int buffer, int x, int y)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + buffer);
		glm::vec4 i(-1);
		auto specs = m_ColorAttachmentSpecs[buffer].m_TextureFormat;
		//add switch
		switch (specs)
		{
		case Hazel::FramebufferTextureFormat::None:
			HZ_CORE_WARN("error, tried to read from a non-color buffer");
			break;
		case Hazel::FramebufferTextureFormat::RGBA8:
			glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &i.x);
			break;
		case Hazel::FramebufferTextureFormat::RED:
			glReadPixels(x, y, 1, 1, GL_RED, GL_FLOAT, &i.x);
			HZ_CORE_WARN("error, tried to read from a single-channel color buffer");
			break;
		case Hazel::FramebufferTextureFormat::RED_INTEGER:
			i.x = Pixel(buffer, x, y);
			HZ_CORE_WARN("error, tried to read from a single-channel color buffer");
			break;
		case Hazel::FramebufferTextureFormat::DEPTH24STENCIL8:
			HZ_CORE_WARN("error, tried to read from a non-color buffer");
			break;
		default:
			break;
		}

		return i;
	}
}