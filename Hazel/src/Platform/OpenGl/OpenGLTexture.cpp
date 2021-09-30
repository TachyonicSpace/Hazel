#include "hzpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

namespace Hazel {

	void OpenGLTexture2D::RenderSettings()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		:m_Width(width), m_Height(height)
	{
		HZ_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	OpenGLTexture2D::OpenGLTexture2D()
		:m_Width(1), m_Height(1)
	{
		HZ_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		RenderSettings();

		uint32_t whiteTextureData = 0xffffffff;
		this->SetData(&whiteTextureData, sizeof(whiteTextureData));
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool error)
		:m_Path(path)
	{
		HZ_PROFILE_FUNCTION();

		if (path == "")
		{
		blankTexture:

			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;

			m_Width = m_Height = 1;

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);


			uint32_t whiteTextureData = 0xffffffff;
			this->SetData(&whiteTextureData, sizeof(whiteTextureData));
			return;
		}

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		if (!data)
		{
			auto path0 = path;

			std::string defaultFilepath = "D:\\Hazel\\Hazel-Nut\\assets\\textures\\";
			/*if (path.size() <= defaultFilepath.size()
				|| path.substr(0, defaultFilepath.size()) != defaultFilepath)*/
			if (path.substr(1, 2) != ":\\")
				(std::string&)path = defaultFilepath + path;

			{
				HZ_PROFILE_SCOPE("OpenGLTexture2D::OpenGLTexture2D(const std::string&): loading texture");

				data = stbi_load(path.c_str(), &width, &height, &channels, 0);
			}
			if (!data)
			{
				if (error)
				{
					HZ_CORE_ERROR("Image Path Not Found: {0}, or: {1}", path0, path);
					throw "invalid Image Path.";
				}
			}
		}
		m_Width = width;
		m_Height = height;

		if (channels == 4)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
		}

		HZ_CORE_ASSERT(m_InternalFormat && m_DataFormat, "format unsupported");

		RenderSettings();

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		HZ_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		HZ_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D::subTexture(const int& xIndex, const int& yIndex,
		const int& xSpriteWidth /*= 128*/, const int& ySpriteHeight /*= 128*/)
	{

		m_TexIndicies = { xIndex, yIndex };
		auto x = m_TexIndicies[0], y = m_TexIndicies[1];

		if (x != -1)
		{
			m_TexCoords[0][0] = ((x + 0) * xSpriteWidth) / m_Width;
			m_TexCoords[1][0] = ((x + 1) * xSpriteWidth) / m_Width;
			m_TexCoords[2][0] = ((x + 1) * xSpriteWidth) / m_Width;
			m_TexCoords[3][0] = ((x + 0) * xSpriteWidth) / m_Width;
		}

		if (y != -1)
		{
			m_TexCoords[0][1] = ((y + 0) * ySpriteHeight) / m_Height;
			m_TexCoords[1][1] = ((y + 0) * ySpriteHeight) / m_Height;
			m_TexCoords[2][1] = ((y + 1) * ySpriteHeight) / m_Height;
			m_TexCoords[3][1] = ((y + 1) * ySpriteHeight) / m_Height;
		}
	}

	const glm::mat4x2& OpenGLTexture2D::GetTextureCoordinates() const
	{
		return m_TexCoords;
	}
	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		HZ_PROFILE_FUNCTION();

		uint32_t bytes = (m_DataFormat == GL_RGBA) ? 4 : 3;
		HZ_CORE_ASSERT(size == m_Width * m_Height * bytes, "Data must be entire texture");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}
}