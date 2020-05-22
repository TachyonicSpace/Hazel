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

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};

		static SceneData* m_sceneData;
	};
}