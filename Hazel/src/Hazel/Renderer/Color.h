#pragma once
#include "glm/glm.hpp"

namespace Hazel
{

	class Color
	{
	public:
		Color();
		Color(int greyScale);
		Color(float greyScale);
		Color(glm::vec4 color);
		Color(glm::vec3 color);
		Color(const std::initializer_list<float>& Element);

		const float* GetHSBA() const { return m_HSBA; }
		float* GetHSBAPointer() { return m_HSBA; }
		Color& SetHsb(int hue);
		Color& SetHsb(float hue);
		Color& SetHsb(glm::vec3 hsb);
		Color& SetHsb(glm::vec4 hsba);
		Color& SetHsb(const std::initializer_list<float>& Element);

		const float* GetRGBA() const { return m_RGBA; }
		float* GetRGBAPointer() { return m_RGBA; }
		Color& SetRgb(int greyscale);
		Color& SetRgb(float greyscale);
		Color& SetRgb(glm::vec3 rgb);
		Color& SetRgb(glm::vec4 rgba);
		Color& SetRgb(const std::initializer_list<float>& Element);

		glm::vec3 GetVec3() { return { m_RGBA[0], m_RGBA[1], m_RGBA[2] }; }
		glm::vec4 GetVec4() { return { m_RGBA[0], m_RGBA[1], m_RGBA[2], m_RGBA[3] }; }
		const glm::vec3 GetVec3() const { return { m_RGBA[0], m_RGBA[1], m_RGBA[2] }; }
		const glm::vec4 GetVec4() const { return { m_RGBA[0], m_RGBA[1], m_RGBA[2], m_RGBA[3] }; }

		const uint32_t GetHex() const;
		uint32_t GetHex();

	private:
		float m_RGBA[4];
		float m_HSBA[4];
	};

}