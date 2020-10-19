#pragma once

#include "glm/glm.hpp"
#include "Hazel/Renderer/Color.h"
#include "Hazel/Renderer/Texture.h"
#include "glm/ext/matrix_transform.hpp"


namespace Hazel
{

	class Shape
	{
		Shape() = default;
		Shape(const Shape& sha) = default;
	};

	class Quad
	{
	public:
		Quad(const glm::vec2& pos, const glm::vec2& size,
			const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
			:m_Color(color), m_Tex(tex), m_TilingFactor(tilingFactor)
		{
			m_Transform = glm::translate(glm::mat4(1), { pos.x, pos.y, 0}) *
				glm::scale(glm::mat4(1), { size.x, size.y, 1 });
		}
		Quad(const glm::vec3& pos, const glm::vec2& size,
			const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
			: m_Color(color), m_Tex(tex), m_TilingFactor(tilingFactor)
		{
			m_Transform = glm::translate(glm::mat4(1), pos) *
				glm::scale(glm::mat4(1), { size.x, size.y, 1 });
		}
		Quad(const glm::vec2& pos, const glm::vec2& size,
			Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
			:m_Color(color), m_Tex(tex), m_TilingFactor(tilingFactor)
		{
			m_Transform = glm::translate(glm::mat4(1), { pos.x, pos.y, 0 }) *
				glm::scale(glm::mat4(1), { size.x, size.y, 1 });
		}
		Quad(const glm::vec3& pos, const glm::vec2& size,
			Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
			:m_Color(color), m_Tex(tex), m_TilingFactor(tilingFactor)
		{
			m_Transform = glm::translate(glm::mat4(1), pos) *
				glm::scale(glm::mat4(1), { size.x, size.y, 1 });
		}
		Quad(const glm::vec2& pos, const glm::vec2& size, const float& radianAngle,
			const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
			: m_Color(color), m_Tex(tex), m_TilingFactor(tilingFactor)
		{
			m_Transform = glm::translate(glm::mat4(1), {pos.x, pos.y, 0}) *
				glm::rotate(glm::mat4(1), radianAngle, { 0, 0, 1 }) *
				glm::scale(glm::mat4(1), { size.x, size.y, 1 });
		}
		Quad(const glm::vec3& pos, const glm::vec2& size, const float& radianAngle,
			const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
			: m_Color(color), m_Tex(tex), m_TilingFactor(tilingFactor)
		{
			m_Transform = glm::translate(glm::mat4(1), pos)*
							glm::rotate(glm::mat4(1), radianAngle, { 0, 0, 1 }) *
							glm::scale(glm::mat4(1), { size.x, size.y, 1 });
		}
		Quad(const glm::vec2& pos, const glm::vec2& size, const float& radianAngle,
			Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
			:m_Color(color), m_Tex(tex), m_TilingFactor(tilingFactor)
		{
			m_Transform = glm::translate(glm::mat4(1), { pos.x, pos.y, 0 }) *
				glm::rotate(glm::mat4(1), radianAngle, { 0, 0, 1 }) *
				glm::scale(glm::mat4(1), { size.x, size.y, 1 });
		}
		Quad(const glm::vec3& pos, const glm::vec2& size, const float& radianAngle,
			Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
			:m_Color(color), m_Tex(tex), m_TilingFactor(tilingFactor)
		{
			m_Transform = glm::translate(glm::mat4(1), pos)*
							glm::rotate(glm::mat4(1), radianAngle, { 0, 0, 1 }) *
							glm::scale(glm::mat4(1), { size.x, size.y, 1 });
		}
		Quad(const glm::mat4& transform = glm::mat4(1.f),
			const Color& color = { 1, 1, 1, 1 }, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
			: m_Color(color), m_Tex(tex), m_TilingFactor(tilingFactor), m_Transform(transform)
		{}
		Quad(const glm::mat4& transform,
			Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
			:m_Color(color), m_Tex(tex), m_TilingFactor(tilingFactor), m_Transform(transform)
		{}
	public:
		glm::mat4 transform() { return m_Transform; }
		glm::mat4 transform() const { return m_Transform; }
		glm::mat4& transformRef() { return m_Transform; }

		Ref<Texture2D> tex() { return m_Tex; }
		Ref<Texture2D> tex() const { return m_Tex; }
		Ref<Texture2D>& texRef() { return m_Tex; }

		float tilingFactor() { return m_TilingFactor; }
		float tilingFactor() const { return m_TilingFactor; }
		float& tilingFactorRef() { return m_TilingFactor; }

		Color color() { return m_Color; }
		Color color() const { return m_Color; }
		Color& colorRef() { return m_Color; }
	private:
		glm::mat4 m_Transform;
		Ref<Texture2D> m_Tex;
		float m_TilingFactor;
		Color m_Color;
	};

}