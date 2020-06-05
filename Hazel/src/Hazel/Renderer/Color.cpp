#include "hzpch.h"
#include "Color.h"
Hazel::Color::Color()
{
	m_RGBA[0] = 0;
	m_RGBA[1] = 0;
	m_RGBA[2] = 0;
	m_RGBA[3] = 0;

	m_HSBA[0] = 0;
	m_HSBA[1] = 0;
	m_HSBA[2] = 0;
	m_HSBA[3] = 0;
}
Hazel::Color::Color(const std::initializer_list<float>& Element)
{
	/*HZ_CORE_ASSERT(Element.size() >= 1 && Element.size() != 2 && Element.size() <= 4, "invalid values for a color");



	int i = 0;
	for (float f : Element)
	{
		m_RGBA[i++] = f;
	}

	if (Element.size() == 1)
		m_RGBA[1] = m_RGBA[0];
	if (Element.size() < 3)
		m_RGBA[2] = m_RGBA[0];
	if (Element.size() < 4)
		m_RGBA[3] = 1;


	if (m_RGBA[0] > 1)
		m_RGBA[0] /= 255;
	if (m_RGBA[1] > 1)
		m_RGBA[1] /= 255;
	if (m_RGBA[2] > 1)
		m_RGBA[2] /= 255;
	if (m_RGBA[3] > 1)
		m_RGBA[3] /= 255;


	float cmin = __min(m_RGBA[0], __min(m_RGBA[1], m_RGBA[2]));
	float cmax = __max(m_RGBA[0], __max(m_RGBA[1], m_RGBA[2]));
	float delta = cmax - cmin;

	if (delta == 0)
		m_HSBA[0] = 0;
	else if (cmax == m_RGBA[0])
		m_HSBA[0] = 60 * fmod((m_RGBA[1] - m_RGBA[2]) / delta, 6);
	else if (cmax == m_RGBA[1])
		m_HSBA[0] = 60 * ((m_RGBA[2] - m_RGBA[0]) / delta + 2);
	else if (cmax == m_RGBA[2])
		m_HSBA[0] = 60 * ((m_RGBA[0] - m_RGBA[1]) / delta + 4);

	m_HSBA[1] = (cmax != 0) ? delta / cmax : 0;
	m_HSBA[2] = cmax;
	m_HSBA[3] = m_RGBA[3];*/

	this->SetRgb(Element);
}
Hazel::Color::Color(glm::vec3 color)
{
	/*if (color.r > 1)
		color.r /= 255;
	if (color.g > 1)
		color.g /= 255;
	if (color.b > 1)
		color.b /= 255;


	m_RGBA[0] = color.r;
	m_RGBA[1] = color.g;
	m_RGBA[2] = color.b;
	m_RGBA[3] = 1;

	float cmin = __min(color.r, __min(color.g, color.b));
	float cmax = __max(color.r, __max(color.g, color.b));
	float delta = cmax - cmin;

	if (delta == 0)
		m_HSBA[0] = 0;
	else if (cmax == color.r)
		m_HSBA[0] = 60 * fmod((color.g - color.b) / delta, 6);
	else if (cmax == color.g)
		m_HSBA[0] = 60 * ((color.b - color.r) / delta + 2);
	else if (cmax == color.b)
		m_HSBA[0] = 60 * ((color.r - color.g) / delta + 4);

	m_HSBA[1] = (cmax != 0) ? delta / cmax : 0;
	m_HSBA[2] = cmax;
	m_HSBA[3] = 1;*/

	this->SetRgb(color);
}
Hazel::Color::Color(glm::vec4 color)
{
	/*if (color.r > 1)
		color.r /= 255;
	if (color.g > 1)
		color.g /= 255;
	if (color.b > 1)
		color.b /= 255;
	if (color.a > 1)
		color.a /= 255;

	m_RGBA[0] = color.r;
	m_RGBA[1] = color.g;
	m_RGBA[2] = color.b;
	m_RGBA[3] = color.a;

	float cmin = __min(color.r, __min(color.g, color.b));
	float cmax = __max(color.r, __max(color.g, color.b));
	float delta = cmax - cmin;

	if (delta == 0)
		m_HSBA[0] = 0;
	else if (cmax == color.r)
		m_HSBA[0] = 60 * fmod((color.g - color.b) / delta, 6);
	else if (cmax == color.g)
		m_HSBA[0] = 60 * ((color.b - color.r) / delta + 2);
	else if (cmax == color.b)
		m_HSBA[0] = 60 * ((color.r - color.g) / delta + 4);

	m_HSBA[1] = (cmax != 0) ? delta / cmax : 0;
	m_HSBA[2] = cmax;
	m_HSBA[3] = color.a;*/
	this->SetRgb(color);
}
Hazel::Color::Color(float greyScale)
{
	/*if (greyScale > 1)
		greyScale /= 255;

	m_RGBA[0] = greyScale;
	m_RGBA[1] = greyScale;
	m_RGBA[2] = greyScale;
	m_RGBA[3] = 1;

	m_HSBA[0] = 0;
	m_HSBA[1] = 0;
	m_HSBA[2] = greyScale;
	m_HSBA[3] = 1;*/
	this->SetRgb(greyScale);
}
Hazel::Color::Color(int greyScale)
{
		/*m_RGBA[0] = (float)(greyScale) / 255.f;
		m_RGBA[1] = (float)(greyScale) / 255.f;
		m_RGBA[2] = (float)(greyScale) / 255.f;
		m_RGBA[3] = 1;


		m_HSBA[0] = 0;
		m_HSBA[1] = 0;
		m_HSBA[2] = (float)(greyScale) / 255.f;
		m_HSBA[3] = 1;*/

	this->SetRgb(greyScale);
}


Hazel::Color& Hazel::Color::SetHsb(int hue)
{
	this->Hazel::Color::SetHsb({ (float)hue, 1, 1 });
	return *this;
}
Hazel::Color& Hazel::Color::SetHsb(const std::initializer_list<float>& Element)
{
	HZ_CORE_ASSERT(Element.size() >= 1 && Element.size() != 2 && Element.size() <= 4, "invalid values for a color");

	int i = 0;
	for (float f : Element)
	{
		m_HSBA[i++] = f;
	}

	if (Element.size() == 1)
		m_HSBA[1] = 1;
	if (Element.size() < 3)
		m_HSBA[2] = 1;
	if (Element.size() < 4)
		m_HSBA[3] = 1;

	if (m_HSBA[0] < 1)
		m_HSBA[0] *= 360;

	int h = (int)m_HSBA[0] % 360;
	float s = m_HSBA[1], b = m_HSBA[2];

	float C = s * b;
	float X = C * (1 - abs(fmod((h / 60.f), 2) - 1));
	float m = b - C;

	if (0 <= h && h < 60)
	{
		m_RGBA[0] = C + m;
		m_RGBA[1] = X + m;
		m_RGBA[2] = 0 + m;
	}
	else if (60 <= h && h < 120)
	{
		m_RGBA[0] = X + m;
		m_RGBA[1] = C + m;
		m_RGBA[2] = 0 + m;
	}
	else if (120 <= h && h < 180)
	{
		m_RGBA[0] = 0 + m;
		m_RGBA[1] = C + m;
		m_RGBA[2] = X + m;
	}
	else if (180 <= h && h < 240)
	{
		m_RGBA[0] = 0 + m;
		m_RGBA[1] = X + m;
		m_RGBA[2] = C + m;
	}
	else if (240 <= h && h < 300)
	{
		m_RGBA[0] = X + m;
		m_RGBA[1] = 0 + m;
		m_RGBA[2] = C + m;
	}
	else if (300 <= h && h < 360)
	{
		m_RGBA[0] = C + m;
		m_RGBA[1] = 0 + m;
		m_RGBA[2] = X + m;
	}
	if (Element.size() < 4)
		m_RGBA[3] = 1;
	else
		m_RGBA[3] = m_HSBA[3];

	return *this;
}
Hazel::Color& Hazel::Color::SetHsb(float hue)
{
	this->Hazel::Color::SetHsb({hue, 1, 1});
	return *this;
}
Hazel::Color& Hazel::Color::SetHsb(glm::vec4 m_HSBA)
{

	if (m_HSBA[0] < 1)
		m_HSBA[0] *= 360;

	int h = (int)m_HSBA[0] % 360;
	float s = m_HSBA[1], b = m_HSBA[2];

	float C = s * b;
	float X = C * (1 - abs(fmod((h / 60.f), 2) - 1));
	float m = b - C;

	if (0 <= h && h < 60)
	{
		m_RGBA[0] = C + m;
		m_RGBA[1] = X + m;
		m_RGBA[2] = 0 + m;
	}
	else if (60 <= h && h < 120)
	{
		m_RGBA[0] = X + m;
		m_RGBA[1] = C + m;
		m_RGBA[2] = 0 + m;
	}
	else if (120 <= h && h < 180)
	{
		m_RGBA[0] = 0 + m;
		m_RGBA[1] = C + m;
		m_RGBA[2] = X + m;
	}
	else if (180 <= h && h < 240)
	{
		m_RGBA[0] = 0 + m;
		m_RGBA[1] = X + m;
		m_RGBA[2] = C + m;
	}
	else if (240 <= h && h < 300)
	{
		m_RGBA[0] = X + m;
		m_RGBA[1] = 0 + m;
		m_RGBA[2] = C + m;
	}
	else if (300 <= h && h < 360)
	{
		m_RGBA[0] = C + m;
		m_RGBA[1] = 0 + m;
		m_RGBA[2] = X + m;
	}
	m_RGBA[3] = m_HSBA[3];

	return *this;
}
Hazel::Color& Hazel::Color::SetHsb(glm::vec3 hsb)
{
	this->SetHsb({ hsb.r, hsb.g, hsb.b, 1 });

	return *this;
}


Hazel::Color& Hazel::Color::SetRgb(int greyScale)
{
	m_RGBA[0] = (float)(greyScale) / 255.f;
	m_RGBA[1] = (float)(greyScale) / 255.f;
	m_RGBA[2] = (float)(greyScale) / 255.f;
	m_RGBA[3] = 1;


	m_HSBA[0] = 0;
	m_HSBA[1] = 0;
	m_HSBA[2] = (float)(greyScale) / 255.f;
	m_HSBA[3] = 1;

	return *this;
}
Hazel::Color& Hazel::Color::SetRgb(const std::initializer_list<float>& Element)
{
	HZ_CORE_ASSERT(Element.size() >= 1 && Element.size() != 2 && Element.size() <= 4, "invalid values for a color");



	int i = 0;
	for (float f : Element)
	{
		m_RGBA[i++] = f;
	}

	if (Element.size() == 1)
		m_RGBA[1] = m_RGBA[0];
	if (Element.size() < 3)
		m_RGBA[2] = m_RGBA[0];
	if (Element.size() < 4)
		m_RGBA[3] = 1;


	if (m_RGBA[0] > 1)
		m_RGBA[0] /= 255;
	if (m_RGBA[1] > 1)
		m_RGBA[1] /= 255;
	if (m_RGBA[2] > 1)
		m_RGBA[2] /= 255;
	if (m_RGBA[3] > 1)
		m_RGBA[3] /= 255;


	float cmin = __min(m_RGBA[0], __min(m_RGBA[1], m_RGBA[2]));
	float cmax = __max(m_RGBA[0], __max(m_RGBA[1], m_RGBA[2]));
	float delta = cmax - cmin;

	if (delta == 0)
		m_HSBA[0] = 0;
	else if (cmax == m_RGBA[0])
		m_HSBA[0] = 60 * fmod((m_RGBA[1] - m_RGBA[2]) / delta, 6);
	else if (cmax == m_RGBA[1])
		m_HSBA[0] = 60 * ((m_RGBA[2] - m_RGBA[0]) / delta + 2);
	else if (cmax == m_RGBA[2])
		m_HSBA[0] = 60 * ((m_RGBA[0] - m_RGBA[1]) / delta + 4);

	m_HSBA[1] = (cmax != 0) ? delta / cmax : 0;
	m_HSBA[2] = cmax;
	m_HSBA[3] = m_RGBA[3];

	return *this;
}
Hazel::Color& Hazel::Color::SetRgb(glm::vec4 color)
{
	if (color.r > 1)
		color.r /= 255;
	if (color.g > 1)
		color.g /= 255;
	if (color.b > 1)
		color.b /= 255;
	if (color.a > 1)
		color.a /= 255;

	m_RGBA[0] = color.r;
	m_RGBA[1] = color.g;
	m_RGBA[2] = color.b;
	m_RGBA[3] = color.a;

	float cmin = __min(color.r, __min(color.g, color.b));
	float cmax = __max(color.r, __max(color.g, color.b));
	float delta = cmax - cmin;

	if (delta == 0)
		m_HSBA[0] = 0;
	else if (cmax == color.r)
		m_HSBA[0] = 60 * fmod((color.g - color.b) / delta, 6);
	else if (cmax == color.g)
		m_HSBA[0] = 60 * ((color.b - color.r) / delta + 2);
	else if (cmax == color.b)
		m_HSBA[0] = 60 * ((color.r - color.g) / delta + 4);

	m_HSBA[1] = (cmax != 0) ? delta / cmax : 0;
	m_HSBA[2] = cmax;
	m_HSBA[3] = color.a;

	return *this;
}
Hazel::Color& Hazel::Color::SetRgb(glm::vec3 color)
{
	if (color.r > 1)
		color.r /= 255;
	if (color.g > 1)
		color.g /= 255;
	if (color.b > 1)
		color.b /= 255;


	m_RGBA[0] = color.r;
	m_RGBA[1] = color.g;
	m_RGBA[2] = color.b;
	m_RGBA[3] = 1;

	float cmin = __min(color.r, __min(color.g, color.b));
	float cmax = __max(color.r, __max(color.g, color.b));
	float delta = cmax - cmin;

	if (delta == 0)
		m_HSBA[0] = 0;
	else if (cmax == color.r)
		m_HSBA[0] = 60 * fmod((color.g - color.b) / delta, 6);
	else if (cmax == color.g)
		m_HSBA[0] = 60 * ((color.b - color.r) / delta + 2);
	else if (cmax == color.b)
		m_HSBA[0] = 60 * ((color.r - color.g) / delta + 4);

	m_HSBA[1] = (cmax != 0) ? delta / cmax : 0;
	m_HSBA[2] = cmax;
	m_HSBA[3] = 1;

	return *this;
}
Hazel::Color& Hazel::Color::SetRgb(float greyScale)
{
	if (greyScale > 1)
		greyScale /= 255;

	m_RGBA[0] = greyScale;
	m_RGBA[1] = greyScale;
	m_RGBA[2] = greyScale;
	m_RGBA[3] = 1;

	m_HSBA[0] = 0;
	m_HSBA[1] = 0;
	m_HSBA[2] = greyScale;
	m_HSBA[3] = 1;

	return *this;
}


const glm::uint32_t Hazel::Color::GetHex() const
{
	uint32_t val = 0;
	val += (uint32_t)(m_RGBA[0] * 255);
	val = val << sizeof(uint32_t) * 2;
	val += (uint32_t)(m_RGBA[1] * 255);
	val = val << sizeof(uint32_t) * 2;
	val += (uint32_t)(m_RGBA[2] * 255);
	val = val << sizeof(uint32_t) * 2;
	val += (uint32_t)(m_RGBA[3] * 255);


	return val;
}
uint32_t Hazel::Color::GetHex()
{
	uint32_t val = 0;
	val += (uint32_t)(m_RGBA[0] * 255);
	val = val << sizeof(uint32_t) * 2;
	val += (uint32_t)(m_RGBA[1] * 255);
	val = val << sizeof(uint32_t) * 2;
	val += (uint32_t)(m_RGBA[2] * 255);
	val = val << sizeof(uint32_t) * 2;
	val += (uint32_t)(m_RGBA[3] * 255);


	return val;
}