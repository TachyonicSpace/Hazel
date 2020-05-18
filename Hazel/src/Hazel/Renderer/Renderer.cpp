#include "hzpch.h"
#include "Renderer.h"

namespace Hazel
{

	void Renderer::BeginScene()
	{

	}
	void Renderer::EndScene()
	{

	}
	void Renderer::Submit(const std::shared_ptr<VertexArray>& va)
	{
		va->Bind();
		RenderCommand::DrawIndexed(va);
	}

}