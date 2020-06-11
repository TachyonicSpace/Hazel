#pragma once

#include "Hazel/Core/Core.h"
#include "glm/glm.hpp"
#include <string>

//#define Texture2D

namespace Hazel
{

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual const glm::mat4x2& GetTextureCoordinates() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};


	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);


		virtual void subTexture(const int& xIndex, const int& yIndex, 
			const int& xSpriteWidth = 128, const int& ySpriteHeight = 128) = 0;
	};

}