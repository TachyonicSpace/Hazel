#include "hzpch.h"
#include "Color.h"

namespace Hazel
{
	bool Color::HSBMode = false;

	//color pallet, used to store and save colors
	std::vector<Color> Color::ColorPallete = { 
		Color({255.f, 255.f, 255.f}),
		Color({000.f, 000.f, 000.f}),
		Color({255.f, 000.f, 000.f}),
		Color({000.f, 255.f, 000.f}),
		Color({000.f, 000.f, 255.f}),
		Color({255.f, 255.f, 000.f}),
		Color({000.f, 255.f, 255.f}),
		Color({255.f, 000.f, 255.f}),
		Color({192.f, 192.f, 192.f}),
		Color({128.f, 128.f, 128.f}),
		Color({128.f, 000.f, 000.f}),
		Color({128.f, 128.f, 000.f}),
		Color({000.f, 128.f, 000.f}),
		Color({128.f, 000.f, 128.f}),
		Color({000.f, 128.f, 128.f}),
		Color({000.f, 000.f, 128.f}),
		Color({139.f, 069.f, 019.f})
	};

	//common colors used as a starting point
	const Color Color::White = Color::ColorPallete[0];
	const Color Color::Black = Color::ColorPallete[1];
	const Color Color::Red = Color::ColorPallete[2];
	const Color Color::Lime = Color::ColorPallete[3];
	const Color Color::Blue = Color::ColorPallete[4];
	const Color Color::Yellow = Color::ColorPallete[5];
	const Color Color::Cyan = Color::ColorPallete[6];
	const Color Color::Magenta = Color::ColorPallete[7];
	const Color Color::Silver = Color::ColorPallete[8];
	const Color Color::Grey = Color::ColorPallete[9];
	const Color Color::Maroon = Color::ColorPallete[10];
	const Color Color::Olive = Color::ColorPallete[11];
	const Color Color::Green = Color::ColorPallete[12];
	const Color Color::Purple = Color::ColorPallete[13];
	const Color Color::Teal = Color::ColorPallete[14];
	const Color Color::Navy = Color::ColorPallete[15];
	const Color Color::Brown = Color::ColorPallete[16];




	//makes the color black by default
	Color::Color()
	{
		r = 0;
		g = 0;
		b = 0;
		a = 1.f;

		h = 0;
		s = 0;
		v = 0;
	}
	//copys color
	Color::Color(const Color& col)
	{
		r = col.r;
		g = col.g;
		b = col.b;
		a = col.a;

		h = col.h;
		s = col.s;
		v = col.v;
	}
	//color from three values
	Color::Color(float R, float G, float B)
	{
		if (HSBMode)
			this->SetHsb(R, G, B);
		else
			this->SetRgb(R, G, B);
	}
	//sets color from 4 values
	Color::Color(float R, float G, float B, float A)
	{
		if (HSBMode)
			this->SetHsb(R, G, B, A);
		else
			this->SetRgb(R, G, B, A);
	}
	//sets colors from initilizer list
	Color::Color(const std::initializer_list<float>& Element)
	{
		if (HSBMode)
			this->SetHsb(Element);
		else
			this->SetRgb(Element);
	}
	//sets color from vec3
	Color::Color(glm::vec3 color)
	{
		if (HSBMode)
			this->SetHsb(color);
		else
			this->SetRgb(color);
	}
	Color::Color(glm::vec4 color)
	{
		if (HSBMode)
			this->SetHsb(color);
		else
			this->SetRgb(color);
	}
	//sets greyscale color
	Color::Color(float greyScale)
	{
		if (HSBMode)
			this->SetHsb(greyScale);
		else
			this->SetRgb(greyScale);
	}
	Color::Color(int greyScale)
	{
		if (HSBMode)
			this->SetHsb(greyScale);
		else
			this->SetRgb(greyScale);
	}

	//makes a color with the hue and full saturation asnd brightness
	Color& Color::SetHsb(int hue)
	{
		this->Color::SetHsb({ (float)hue, 1, 1 });
		return *this;
	}
	Color& Color::SetHsb(const std::initializer_list<float>& Element)
	{
		HZ_CORE_ASSERT(Element.size() >= 1 && Element.size() != 2 && Element.size() <= 4, "invalid values for a color");

		int i = 0;
		for (float f : Element)
		{
			if (i == 3)
				m_RGBA[i] = f;
			else
				m_HSV[i++] = f;
		}

		if (Element.size() == 1)
			s = 1;
		if (Element.size() < 3)
			v = 1;

		if (h < 1)
			h *= 360;

		h = fmod(h, 360.f);
		s = __min(1.f, __max(s, 0.f));
		v = __min(1.f, __max(v, 0.f));

		float C = s * v;
		float X = C * (1 - abs((float)fmod((h / 60.f), 2) - 1));
		float m = v - C;

		if (0 <= h && h < 60)
		{
			r = C + m;
			g = X + m;
			b = 0 + m;
		}
		else if (60 <= h && h < 120)
		{
			r = X + m;
			g = C + m;
			b = 0 + m;
		}
		else if (120 <= h && h < 180)
		{
			r = 0 + m;
			g = C + m;
			b = X + m;
		}
		else if (180 <= h && h < 240)
		{
			r = 0 + m;
			g = X + m;
			b = C + m;
		}
		else if (240 <= h && h < 300)
		{
			r = X + m;
			g = 0 + m;
			b = C + m;
		}
		else if (300 <= h && h < 360)
		{
			r = C + m;
			g = 0 + m;
			b = X + m;
		}
		if (Element.size() < 4)
			a = 1;

		return *this;
	}
	Color& Color::SetHsb(float hue)
	{
		this->Color::SetHsb({ hue, 1, 1 });
		return *this;
	}
	Color& Color::SetHsb(glm::vec4 m_HSBA)
	{

		/*if (h < 1)
			h *= 360;

		h = (int)h % 360;
		float s = s, v = m_HSV[2];

		float C = s * v;
		float X = C * (1 - abs((float)fmod((h / 60.f), 2) - 1));
		float m = v - C;

		if (0 <= h && h < 60)
		{
			r = C + m;
			g = X + m;
			b = 0 + m;
		}
		else if (60 <= h && h < 120)
		{
			r = X + m;
			g = C + m;
			b = 0 + m;
		}
		else if (120 <= h && h < 180)
		{
			r = 0 + m;
			g = C + m;
			b = X + m;
		}
		else if (180 <= h && h < 240)
		{
			r = 0 + m;
			g = X + m;
			b = C + m;
		}
		else if (240 <= h && h < 300)
		{
			r = X + m;
			g = 0 + m;
			b = C + m;
		}
		else if (300 <= h && h < 360)
		{
			r = C + m;
			g = 0 + m;
			b = X + m;
		}*/

		this->SetHsb({ h,s,m_HSV[2] });
		return *this;
	}
	Color& Color::SetHsb(glm::vec3 hsb)
	{
		this->SetHsb({ hsb.r, hsb.g, hsb.b, 1 });

		return *this;
	}
	Color& Color::SetHsb(float R, float G, float B)
	{
		this->SetHsb({ (float)R, (float)G, (float)B });
		return *this;
	}
	Color& Color::SetHsb(float R, float G, float B, float A)
	{
		this->SetHsb({ (float)R, (float)G, (float)B, (float)A });
		return *this;
	}
	Color& Color::SetHsb(const Color& col)
	{
		this->SetHsb({ col.h, col.s, col.m_HSV[2] });
		return *this;
	}



	Color& Color::SetRgb(int greyScale)
	{
		r = (float)(greyScale) / 255.f;
		g = (float)(greyScale) / 255.f;
		b = (float)(greyScale) / 255.f;
		a = 1;


		h = 0;
		s = 0;
		m_HSV[2] = (float)(greyScale) / 255.f;

		return *this;
	}
	Color& Color::SetRgb(const std::initializer_list<float>& Element)
	{
		HZ_CORE_ASSERT(Element.size() >= 1 && Element.size() != 2 && Element.size() <= 4, "invalid values for a color");



		int i = 0;
		for (float f : Element)
		{
			m_RGBA[i++] = f;
		}

		if (Element.size() == 1)
			g = r;
		if (Element.size() < 3)
			b = r;
		if (Element.size() < 4)
			a = 1;


		if (r > 1)
			r /= 255.f;
		if (g > 1)
			g /= 255.f;
		if (b > 1)
			b /= 255.f;
		if (a > 1)
			a /= 255.f;


		float cmin = __min(r, __min(g, b));
		float cmax = __max(r, __max(g, b));
		float delta = cmax - cmin;

		if (delta == 0)
			h = 0;
		else if (cmax == r)
			h = 60 * (float)fmod((g - b) / delta, 6);
		else if (cmax == g)
			h = 60 * ((b - r) / delta + 2);
		else if (cmax == b)
			h = 60 * ((r - g) / delta + 4);

		s = (cmax != 0) ? delta / cmax : 0;
		m_HSV[2] = cmax;

		return *this;
	}
	Color& Color::SetRgb(glm::vec4 color)
	{
		/*if (color.r > 1)
			color.r /= 255;
		if (color.g > 1)
			color.g /= 255;
		if (color.b > 1)
			color.b /= 255;
		if (color.a > 1)
			color.a /= 255;

		r = color.r;
		g = color.g;
		b = color.b;
		a = color.a;

		float cmin = __min(color.r, __min(color.g, color.b));
		float cmax = __max(color.r, __max(color.g, color.b));
		float delta = cmax - cmin;

		if (delta == 0)
			h = 0;
		else if (cmax == color.r)
			h = 60 * (float)fmod((color.g - color.b) / delta, 6);
		else if (cmax == color.g)
			h = 60 * ((color.b - color.r) / delta + 2);
		else if (cmax == color.b)
			h = 60 * ((color.r - color.g) / delta + 4);

		s = (cmax != 0) ? delta / cmax : 0;
		m_HSV[2] = cmax;*/

		SetRgb({ color.r, color.g, color.b, color.a });

		return *this;
	}
	Color& Color::SetRgb(glm::vec3 color)
	{
		/*if (color.r > 1)
			color.r /= 255;
		if (color.g > 1)
			color.g /= 255;
		if (color.b > 1)
			color.b /= 255;


		r = color.r;
		g = color.g;
		b = color.b;
		a = 1;

		float cmin = __min(color.r, __min(color.g, color.b));
		float cmax = __max(color.r, __max(color.g, color.b));
		float delta = cmax - cmin;

		if (delta == 0)
			h = 0;
		else if (cmax == color.r)
			h = 60 * (float)fmod((color.g - color.b) / delta, 6);
		else if (cmax == color.g)
			h = 60 * ((color.b - color.r) / delta + 2);
		else if (cmax == color.b)
			h = 60 * ((color.r - color.g) / delta + 4);

		s = (cmax != 0) ? delta / cmax : 0;
		m_HSV[2] = cmax;*/

		SetRgb({ color.r, color.g, color.b });

		return *this;
	}
	Color& Color::SetRgb(float greyScale)
	{
		/*if (greyScale > 1)
			greyScale /= 255;

		r = greyScale;
		g = greyScale;
		b = greyScale;
		a = 1;

		h = 0;
		s = 0;
		m_HSV[2] = greyScale;*/
		SetRgb({ greyScale });

		return *this;
	}
	Color& Color::SetRgb(float R, float G, float B)
	{
		this->SetRgb({ (float)R, (float)G, (float)B });
		return *this;
	}
	Color& Color::SetRgb(float R, float G, float B, float A)
	{
		this->SetRgb({ (float)R, (float)G, (float)B, (float)A });
		return *this;
	}
	Color& Color::SetRgb(const Color& col)
	{
		this->SetRgb({ col.r, col.g, col.b, col.a });
		return *this;
	}


	//gets the hex representation of the rgba values
	const uint32_t Color::GetHex() const
	{
		uint32_t val = 0;
		val += (uint32_t)(r * 255);
		val = val << sizeof(uint32_t) * 2;
		val += (uint32_t)(g * 255);
		val = val << sizeof(uint32_t) * 2;
		val += (uint32_t)(b * 255);
		val = val << sizeof(uint32_t) * 2;
		val += (uint32_t)(a * 255);


		return val;
	}
	//gets the hex representation of the rgba values
	uint32_t Color::GetHex()
	{
		uint32_t val = 0;
		val += (uint32_t)(r * 255);
		val = val << sizeof(uint32_t) * 2;
		val += (uint32_t)(g * 255);
		val = val << sizeof(uint32_t) * 2;
		val += (uint32_t)(b * 255);
		val = val << sizeof(uint32_t) * 2;
		val += (uint32_t)(a * 255);


		return val;
	}
	//gets the color at the ith indez
	float& Color::operator[](int i)
	{
		if (HSBMode && (i = abs(i)) < 3)
			return this->m_HSV[i];
		else
			return this->m_RGBA[i];
	}

	//subtracts colors together, maxing each value at 1
	Hazel::Color Color::operator-=(const Color& col)
	{
		for (int i = 0; i < 3; i++)
		{
			m_RGBA[i] = m_RGBA[i] - col.m_RGBA[i];
			m_RGBA[i] = __min(1.f, __max(m_RGBA[i], 0.f));
		}
		return *this;
	}
	//adds colors together, maxing each value at 1
	Hazel::Color Color::operator+=(const Color& col)
	{
		for (int i = 0; i < 3; i++)
		{
			m_RGBA[i] = m_RGBA[i] + col.m_RGBA[i];
			m_RGBA[i] = __min(1.f, __max(m_RGBA[i], 0.f));
		}
		return *this;
	}
	Color Color::operator+(const Color& col)
	{
		float cols[3];
		for (int i = 0; i < 3; i++)
		{
			cols[i] = m_RGBA[i] + col.m_RGBA[i];
			cols[i] = __min(1, __max(cols[i], 0));
		}
		return Color({ cols[0], cols[1], cols[2], 1 });
	}
	Color Color::operator-(const Color& col)
	{
		float cols[3];
		for (int i = 0; i < 3; i++)
		{
			cols[i] = m_RGBA[i] - col.m_RGBA[i];
			cols[i] = __min(1, __max(cols[i], 0));
		}
		return Color({ cols[0], cols[1], cols[2], a });
	}
	//subtracts color from white
	Color Color::operator-() const
	{
		Color tmp = (Color({ 1.f, 1.f, 1.f, a}) - *this);
		tmp.a = a;
		return tmp;
	}

	//adjust the brightness by a value
	Hazel::Color Color::operator*=(float hue)
	{
		if (v * hue != 0)
			SetHsb({ h, s, v * hue, a });
		return *this;
	}
	Hazel::Color Color::operator/=(float hue)
	{
		if (v / hue != 0)
			SetHsb({ h, s, v / hue, a });
		return *this;
	}
	Hazel::Color Color::operator*(float hue)
	{
		auto tmp = *this;
		if (v * hue >= 1 / 255.f)
			tmp.SetHsb({ h, s, v * hue, a });
		else
			tmp = Black;
		return tmp;
	}
	Hazel::Color Color::operator/(float hue)
	{
		auto tmp = Color({ r, g, b, a });
		if (v / hue != 0)
			tmp.SetHsb({ h, s, v / hue, a });
		return tmp;
	}

	Color Color::operator+(float hue)
	{
		auto tmp = Color(*this);
		tmp.SetHsb((float)fmod(h + hue, 360));
		return tmp;
	}
	Color Color::operator-(float hue)
	{
		auto tmp = Color();
		tmp.SetHsb(h - hue + 360 * 360);
		return tmp;
	}
	Color Color::operator+=(float hue)
	{
		this->SetHsb((float)fmod(h + hue, 360));
		return *this;
	}
	Color Color::operator-=(float hue)
	{
		this->SetHsb(h - hue + 360 * 360);
		return *this;
	}

	Color Color::operator=(const Color& col)
	{
		r = col.r;
		g = col.g;
		b = col.b;
		a = col.a;

		h = col.h;
		s = col.s;
		v = col.v;
		return *this;
	}
}