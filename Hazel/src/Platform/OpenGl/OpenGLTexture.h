#pragma once
#include "Hazel/Renderer/Texture.h"

#include "glad/glad.h"

namespace Hazel {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path, bool error = true);
		OpenGLTexture2D();
		~OpenGLTexture2D();

		void RenderSettings();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool operator ==(const Texture& other) const override 
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID; 
		}


		virtual const glm::mat4x2& GetTextureCoordinates() const override;
		virtual void subTexture(const int& xIndex, const int& yIndex,
			const int& xSpriteWidth = 128, const int& ySpriteHeight = 128) override;

	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat = 0, m_DataFormat = 0;

		glm::mat4x2 m_TexCoords = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };
		glm::vec2 m_TexIndicies = { -1, -1 };

	};

}