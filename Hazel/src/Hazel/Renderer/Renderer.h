#pragma once

#include "RenderCommand.h"

#include "Camera.h"

#include "Shader.h"

namespace Hazel
{

	class Renderer
	{
	public:
		static void Init(bool blend = true);
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};

		static Scope<SceneData> m_sceneData;
	};
}