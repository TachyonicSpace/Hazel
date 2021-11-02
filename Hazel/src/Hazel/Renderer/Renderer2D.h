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
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f);
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = {1, 1, 1, 1});
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = {1, 1, 1, 1});
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const float& radianAngle,	const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const float& radianAngle,	const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f);
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const float& radianAngle,	Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 });
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const float& radianAngle,	Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 });
		
		static void DrawQuad(uint32_t ID, const glm::vec2& pos, const glm::vec2& size, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f);
		static void DrawQuad(uint32_t ID, const glm::vec3& pos, const glm::vec2& size, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f);
		static void DrawQuad(uint32_t ID, const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 });
		static void DrawQuad(uint32_t ID, const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 });
		static void DrawQuad(uint32_t ID, const glm::vec2& pos, const glm::vec2& size, const float& radianAngle, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f);
		static void DrawQuad(uint32_t ID, const glm::vec3& pos, const glm::vec2& size, const float& radianAngle, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f);
		static void DrawQuad(uint32_t ID, const glm::vec2& pos, const glm::vec2& size, const float& radianAngle, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 });
		static void DrawQuad(uint32_t ID, const glm::vec3& pos, const glm::vec2& size, const float& radianAngle, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 });


		static void DrawQuad(const glm::mat4& transform, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f);
		static void DrawQuad(const glm::mat4& transform, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 });

		static void DrawQuad(uint32_t ID, const glm::mat4& transform, const Color& color, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f);
		static void DrawQuad(uint32_t ID, const glm::mat4& transform, Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& color = { 1, 1, 1, 1 });

		static void DrawSprite(const glm::mat4& transform, Component::SpriteRenderer& src, int entityID);

		//stats
		struct Statistics
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;
			uint32_t quadsPerDrawCall = 0;
		};

		static void ResetStats();
		static Statistics& GetStats();
	};

}

