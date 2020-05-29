#pragma once

#include "Camera.h"
#include "Texture.h"

namespace Hazel {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& cam);
		static void EndScene();

		//primatives
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, const Ref<Texture>& tex = nullptr);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, const Ref<Texture>& tex = nullptr);

		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, 
			const Ref<Texture2D>& tex, const glm::vec4& color = {1, 1, 1, 1});
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, 
			const Ref<Texture2D>& tex, const glm::vec4& color = {1, 1, 1, 1});
	};

}

