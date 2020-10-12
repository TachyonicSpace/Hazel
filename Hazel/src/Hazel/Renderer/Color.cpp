#include "hzpch.h"
#include "Color.h"

namespace Hazel
{
	bool Color::HSBMode = false;
	
	const Color Color::Black(0, 0, 0);
	const Color Color::White(255, 255, 255);
	const Color Color::Red(255, 0, 0);
	const Color Color::Lime(0, 255, 0);
	const Color Color::Blue(0, 0, 255);
	const Color Color::Yellow(255, 255, 0);
	const Color Color::Cyan(0, 255, 255);
	const Color Color::Magenta(255, 0, 255);
	const Color Color::Silver(192, 192, 192);
	const Color Color::Grey(128, 128, 128);
	const Color Color::Maroon(128, 0, 0);
	const Color Color::Olive(128, 128, 0);
	const Color Color::Green(0, 128, 0);
	const Color Color::Purple(128, 0, 128);
	const Color Color::Teal(0, 128, 128);
	const Color Color::Navy(0, 0, 128);
	const Color Color::Brown(139, 69, 19);



	//#define old

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
	Color::Color(int R, int G, int B)
	{
		if (HSBMode)
			this->SetHsb(R, G, B);
		else
			this->SetRgb(R, G, B);
	}
	Color::Color(int R, int G, int B, int A)
	{
		if (HSBMode)
			this->SetHsb(R, G, B, A);
		else
			this->SetRgb(R, G, B, A);
	}
	Color::Color(const std::initializer_list<float>& Element)
	{
		#ifdef old
		HZ_CORE_ASSERT(Element.size() >= 1 && Element.size() != 2 && Element.size() <= 4, "invalid values for A color");



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
			r /= 255;
		if (g > 1)
			g /= 255;
		if (b > 1)
			b /= 255;
		if (a > 1)
			a /= 255;


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

		#else
		if (HSBMode)
			this->SetHsb(Element);
		else
			this->SetRgb(Element);
		#endif
	}
	Color::Color(glm::vec3 color)
	{
		#ifdef old

		if (color.r > 1)
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
		m_HSV[2] = cmax;

		#else
		if (HSBMode)
			this->SetHsb(color);
		else
			this->SetRgb(color);
		#endif
	}
	Color::Color(glm::vec4 color)
	{
		#ifdef old
		if (color.r > 1)
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
		m_HSV[2] = cmax;

		#else
		if (HSBMode)
			this->SetHsb(color);
		else
			this->SetRgb(color);
		#endif
	}
	Color::Color(float greyScale)
	{
		#ifdef old
		if (greyScale > 1)
			greyScale /= 255;

		r = greyScale;
		g = greyScale;
		b = greyScale;
		a = 1;

		h = 0;
		s = 0;
		m_HSV[2] = greyScale;
		#else
		if (HSBMode)
			this->SetHsb(greyScale);
		else
			this->SetRgb(greyScale);
		#endif
	}
	Color::Color(int greyScale)
	{
		#ifdef old
		r = (float)(greyScale) / 255.f;
		g = (float)(greyScale) / 255.f;
		b = (float)(greyScale) / 255.f;
		a = 1;


		h = 0;
		s = 0;
		m_HSV[2] = (float)(greyScale) / 255.f;

		#else

		if (HSBMode)
			this->SetHsb(greyScale);
		else
			this->SetRgb(greyScale);
		#endif
	}


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
			m_HSV[i++] = f;
		}

		if (Element.size() == 1)
			s = 1;
		if (Element.size() < 3)
			v = 1;

		if (h < 1)
			h *= 360;

		h = fmod(h, 360.f);

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


	Color& Color::SetHsb(int R, int G, int B)
	{
		this->SetHsb({ (float)R, (float)G, (float)B });
		return *this;
	}
	Color& Color::SetHsb(int R, int G, int B, int A)
	{
		this->SetHsb({ (float)R, (float)G, (float)B, (float)A });
		return *this;
	}
	Color& Color::SetHsb(const Color& col)
	{
		this->SetHsb(col.h, col.s, col.m_HSV[2]);
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


	Color& Color::SetRgb(int R, int G, int B)
	{
		this->SetRgb({ (float)R, (float)G, (float)B });
		return *this;
	}
	Color& Color::SetRgb(int R, int G, int B, int A)
	{
		this->SetRgb({ (float)R, (float)G, (float)B, (float)A });
		return *this;
	}
	Color& Color::SetRgb(const Color& col)
	{
		this->SetRgb(col.r, col.g, col.b, col.a);
		return *this;
	}
	const glm::uint32_t Color::GetHex() const
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
	float& Color::operator[](int i)
	{
		if (HSBMode)
			return this->m_HSV[i];
		else
			return this->m_RGBA[i];
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
		return Color({ cols[0], cols[1], cols[2], 1.f });
	}
	Color Color::operator-() const
	{
		//Color Color::operator-() const
		//{
			return Color(1.f) - *this;
		//}
		//return (((Color)White - *this).a = this->a);
	}
	template<typename T>
	Color Color::operator+(T hue)
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