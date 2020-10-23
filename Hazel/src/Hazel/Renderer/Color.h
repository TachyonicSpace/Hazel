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
		Color(glm::vec3 color);
		Color(glm::vec4 color);
		Color(int r, int g, int b);
		Color(int r, int g, int b, int a);
		Color(const Color& col);
		Color(const std::initializer_list<float>& Element);

		const float* GetHSBA() const { return m_HSV; }
		float* GetHSBAPointer() { return m_HSV; }
		Color& SetHsb(int hue);
		Color& SetHsb(float hue);
		Color& SetHsb(glm::vec3 hsb);
		Color& SetHsb(glm::vec4 hsba);
		Color& SetHsb(const std::initializer_list<float>& Element);
		Color& SetHsb(int r, int g, int b);
		Color& SetHsb(int r, int g, int b, int a);
		Color& SetHsb(const Color& col);

		const float* GetRGBA() const { return m_RGBA; }
		float* GetRGBAPointer() { return m_RGBA; }
		Color& SetRgb(int greyscale);
		Color& SetRgb(float greyscale);
		Color& SetRgb(glm::vec3 rgb);
		Color& SetRgb(glm::vec4 rgba);
		Color& SetRgb(const std::initializer_list<float>& Element);
		Color& SetRgb(int r, int g, int b);
		Color& SetRgb(int r, int g, int b, int a);
		Color& SetRgb(const Color& col);

		glm::vec3 GetVec3() { return { m_RGBA[0], m_RGBA[1], m_RGBA[2] }; }
		glm::vec4 GetVec4() { return { m_RGBA[0], m_RGBA[1], m_RGBA[2], m_RGBA[3] }; }
		const glm::vec3 GetVec3() const { return { m_RGBA[0], m_RGBA[1], m_RGBA[2] }; }
		const glm::vec4 GetVec4() const { return { m_RGBA[0], m_RGBA[1], m_RGBA[2], m_RGBA[3] }; }

		const uint32_t GetHex() const;
		uint32_t GetHex();

		bool& GetMode() { return HSBMode; }
		bool& GetMode() const { return HSBMode; }
		const bool GetConstMode() { return HSBMode; }
		const bool GetConstMode() const { return HSBMode; }

		void SetModeRGBA() { HSBMode = true; }
		void SetModeHSBA() { HSBMode = false; }
		bool FlipMode() { return (HSBMode = !HSBMode); }

		Color operator+(const Color& col);
		Color operator-(const Color& col);
		Color operator+=(const Color& col);
		Color operator-=(const Color& col);
		Color operator-() const;

		Color operator=(const Color& col);
		Color operator==(const Color& col) { return !(this->GetVec4() == col.GetVec4()); }
		Color operator!=(const Color& col) { return !(this->GetVec4() == col.GetVec4()); }
		float& operator[](int i);


		Color operator+(float hue);
		Color operator-(float hue);
		Color operator+(short hue) { return operator+((float)hue); }
		Color operator-(short hue) {return operator-((float)hue);}
		Color operator+(int hue) {return operator+((float)hue);}
		Color operator-(int hue) {return operator-((float)hue);}
		Color operator+(double hue) {return operator+((float)hue);}
		Color operator-(double hue) {return operator-((float)hue);}
		Color operator+(long hue) {return operator+((float)hue);}
		Color operator-(long hue) {return operator-((float)hue);}

		Color operator+=(float hue);
		Color operator-=(float hue);
		Color operator+=(short hue) { return operator+=((float)hue); }
		Color operator-=(short hue) { return operator-=((float)hue); }
		Color operator+=(int hue) { return operator+=((float)hue); }
		Color operator-=(int hue) { return operator-=((float)hue); }
		Color operator+=(double hue) { return operator+=((float)hue); }
		Color operator-=(double hue) { return operator-=((float)hue); }
		Color operator+=(long hue) { return operator+=((float)hue); }
		Color operator-=(long hue) { return operator-=((float)hue); }


		Color operator*(float hue);
		Color operator/(float hue);
		Color operator*(short hue) { return operator*((float)hue); }
		Color operator/(short hue) { return operator/((float)hue); }
		Color operator*(int hue) { return operator*((float)hue); }
		Color operator/(int hue) { return operator/((float)hue); }
		Color operator*(double hue) { return operator*((float)hue); }
		Color operator/(double hue) { return operator/((float)hue); }
		Color operator*(long hue) { return operator*((float)hue); }
		Color operator/(long hue) { return operator/((float)hue); }

		Color operator*=(float hue);
		Color operator/=(float hue);
		Color operator*=(short hue) { return operator*=((float)hue); }
		Color operator/=(short hue) { return operator/=((float)hue); }
		Color operator*=(int hue) { return operator*=((float)hue); }
		Color operator/=(int hue) { return operator/=((float)hue); }
		Color operator*=(double hue) { return operator*=((float)hue); }
		Color operator/=(double hue) { return operator/=((float)hue); }
		Color operator*=(long hue) { return operator*=((float)hue); }
		Color operator/=(long hue) { return operator/=((float)hue); }
	private:
		float m_RGBA[4] = { 0, 0, 0, 1 };
		float m_HSV[3] = { 0, 0, 1 };
		static bool HSBMode;

		float& r = m_RGBA[0];
		float& g = m_RGBA[1];
		float& b = m_RGBA[2];
		float& a = m_RGBA[3];
		float& h = m_HSV[0];
		float& s = m_HSV[1];
		float& v = m_HSV[2];
	public:
		static Color ColorPallete[17];
		static const Color& White	;//(255, 255, 255);
		static const Color& Black	;//(0, 0, 0);
		static const Color& Red		;//(255, 0, 0);
		static const Color& Lime		;//(0, 255, 0);
		static const Color& Blue		;//(0, 0, 255);
		static const Color& Yellow	;//(255, 255, 0);
		static const Color& Cyan		;//(0, 255, 255);
		static const Color& Magenta	;//(255, 0, 255);
		static const Color& Silver	;//(192, 192, 192);
		static const Color& Grey		;//(128, 128, 128);
		static const Color& Maroon	;//(128, 0, 0);
		static const Color& Olive	;//(128, 128, 0);
		static const Color& Green	;//(0, 128, 0);
		static const Color& Purple	;//(128, 0, 128);
		static const Color& Teal		;//(0, 128, 128);
		static const Color& Navy		;//(0, 0, 128);
		static const Color& Brown	;//(139, 69, 19);



	};
}