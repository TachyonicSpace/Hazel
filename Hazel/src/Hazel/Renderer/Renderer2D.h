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
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<Texture2D>& tex);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<Texture2D>& tex);
	};

}

