#include "hzpch.h"
#include "Renderer.h"

namespace Hazel
{
	Renderer::SceneData* Renderer::m_sceneData = new SceneData();

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_sceneData->viewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::EndScene()
	{

	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& va, const glm::mat4& transform)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", m_sceneData->viewProjectionMatrix);
		shader->UploadUniformMat4("u_Transform", transform);

		va->Bind();
		RenderCommand::DrawIndexed(va);
	}

}