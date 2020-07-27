#include "hzpch.h"
#include "Renderer.h"
#include "Renderer2D.h"

namespace Hazel
{
	Scope<Renderer::SceneData> Renderer::m_sceneData = NewScope<Renderer::SceneData>();

	void Renderer::Init(bool blend)
	{
		HZ_PROFILE_FUNCTION();

		RenderCommand::Init(blend);
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_sceneData->viewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::EndScene()
	{

	}
	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", m_sceneData->viewProjectionMatrix);
		shader->UploadUniformMat4("u_Transform", transform);

		va->Bind();
		RenderCommand::DrawIndexed(va);
	}

}