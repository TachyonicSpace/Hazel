#pragma once

#include "Camera.h"
#include "Hazel/Scene/Components.h"

namespace Hazel {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const OrthographicCamera& cam);
		static void BeginScene(const EditorCamera& cam);
		static void EndScene();
		static void Flush();

		//primatives
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
		{
			DrawQuad({ pos.x, pos.y, 0 }, size, color, tex, tilingFactor);
		}
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
		{
			DrawQuad(pos, size, 0, color, tex, tilingFactor);
		}
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
		{
			DrawQuad({ pos.x, pos.y, 0 }, size, 0, tex, tilingFactor, color);
		}
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
		{
			DrawQuad(pos, size, 0, (Ref<Texture2D>&)tex, tilingFactor, color);
		}
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const float& radianAngle, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
		{
			DrawQuad({ pos.x, pos.y, 0 }, size, radianAngle, (Ref<Texture2D>&)tex, tilingFactor, color);
		}
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const float& radianAngle, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
		{
			DrawQuad({ pos.x, pos.y, 0 }, size, radianAngle, (Ref<Texture2D>&)tex, tilingFactor, color);
		}
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const float& radianAngle, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
		{
			DrawQuad({ pos.x, pos.y, 0 }, size, radianAngle, (Ref<Texture2D>&)tex, tilingFactor, color);
		}
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const float& radianAngle, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
		{

			HZ_PROFILE_FUNCTION();

			glm::mat4 transform;
			if (radianAngle == 0)
			{
				transform = glm::translate(glm::mat4(1), pos) *
					glm::scale(glm::mat4(1), { size.x, size.y, 1 });
			}
			else
			{
				transform = glm::translate(glm::mat4(1), pos) *
					glm::rotate(glm::mat4(1), radianAngle, { 0, 0, 1 }) *
					glm::scale(glm::mat4(1), { size.x, size.y, 1 });
			}

			DrawQuad(-1, transform, tex, tilingFactor, color);

		}
		



		static void DrawQuad(uint32_t ID, const glm::vec2& pos, const glm::vec2& size, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
		{
			DrawQuad(ID, { pos.x, pos.y, 0 }, size, color, tex, tilingFactor);
		}
		static void DrawQuad(uint32_t ID, const glm::vec3& pos, const glm::vec2& size, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
		{
			DrawQuad(ID, pos, size, 0, color, tex, tilingFactor);
		}
		static void DrawQuad(uint32_t ID, const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
		{
			DrawQuad(ID, { pos.x, pos.y, 0 }, size, 0, tex, tilingFactor, color);
		}
		static void DrawQuad(uint32_t ID, const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
		{
			DrawQuad(ID, pos, size, 0, (Ref<Texture2D>&)tex, tilingFactor, color);
		}
		static void DrawQuad(uint32_t ID, const glm::vec2& pos, const glm::vec2& size, const float& radianAngle, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
		{
			DrawQuad(ID, { pos.x, pos.y, 0 }, size, radianAngle, (Ref<Texture2D>&)tex, tilingFactor, color);
		}
		static void DrawQuad(uint32_t ID, const glm::vec3& pos, const glm::vec2& size, const float& radianAngle, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
		{
			DrawQuad(ID, { pos.x, pos.y, 0 }, size, radianAngle, (Ref<Texture2D>&)tex, tilingFactor, color);
		}
		static void DrawQuad(uint32_t ID, const glm::vec2& pos, const glm::vec2& size, const float& radianAngle, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 })
		{
			DrawQuad(ID, { pos.x, pos.y, 0 }, size, radianAngle, (Ref<Texture2D>&)tex, tilingFactor, color);
		}
		static void DrawQuad(uint32_t ID, const glm::vec3& pos, const glm::vec2& size, const float& radianAngle, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 });


		static void Renderer2D::DrawQuad(const glm::mat4& transform, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
		{
			DrawQuad(-1, transform, (Ref<Texture2D>)tex, tilingFactor, color);
		}
		static void Renderer2D::DrawQuad(const glm::mat4& transform, Ref<Texture2D>& tex, float tilingFactor, const Color& color)
		{
			DrawQuad(-1, transform, (Ref<Texture2D>)tex, tilingFactor, color);
		}

		static void Renderer2D::DrawQuad(uint32_t ID, const glm::mat4& transform, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
		{
			DrawQuad(ID, transform, (Ref<Texture2D>)tex, tilingFactor, color);
		}
		static void DrawQuad(uint32_t ID, const glm::mat4& transform, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 });

		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

		static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const Color& color, int entityID = -1);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const Color& color, int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const Color& color, int entityID = -1);

		static void DrawSprite(const glm::mat4& transform, Components::SpriteRenderer& src, int entityID);

		//stats
		struct Statistics
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;
			uint32_t quadsPerDrawCall = 0;
		};

		static float GetLineWidth();
		static void SetLineWidth(float width);


		static void ResetStats();
		static Statistics& GetStats();
	};

}

