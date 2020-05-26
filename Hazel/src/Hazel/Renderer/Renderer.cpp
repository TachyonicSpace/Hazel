#include "hzpch.h"
#include "Renderer.h"

#include "Platform/OpenGl/OpenGLShader.h"

namespace Hazel
{
	Renderer::SceneData* Renderer::m_sceneData = new SceneData();

	Ref<ShaderLibrary> Renderer::m_Library = std::make_shared<ShaderLibrary>();

	void Renderer::Init(bool blend)
	{
		RenderCommand::Init(blend);
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
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_sceneData->viewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		va->Bind();
		RenderCommand::DrawIndexed(va);
	}

	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return m_Library;
	}

}