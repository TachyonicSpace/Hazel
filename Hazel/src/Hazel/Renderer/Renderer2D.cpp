#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"


namespace Hazel {

	struct Renderer2DStorage
	{
		Ref<VertexArray> va;
		Ref<Shader> flatColor;

		Ref<Shader> texShader;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage;

		float squareVertices[5 * 4] =
		{
			-.5f, -.5f, 0.f, 0, 0,
			 .5f, -.5f, 0.f, 1, 0,
			 .5f,  .5f, 0.f, 1, 1,
			-.5f,  .5f, 0.f, 0, 1
		};

		s_Data->va = (VertexArray::Create());

		BufferLayout squareLayout = {
			{ShaderDataType::Float3, "position"},
			{ShaderDataType::Float2, "texCoord"}
		};

		Ref<VertexBuffer> squarevb;
		squarevb = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squarevb->SetLayout(squareLayout);
		s_Data->va->AddVertexBuffer(squarevb);

		uint32_t squareIndices[6] = { 0, 1, 2,
									  2, 3, 0 };
		Ref<IndexBuffer> squareib;
		squareib = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(squareIndices[0]));
		s_Data->va->AddIndexBuffer(squareib);

		s_Data->flatColor = Shader::Create("assets/shaders/FlatColor.glsl");
		s_Data->texShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data->texShader->Bind();
		s_Data->texShader->UploadUniformInt("u_Texture", 0);
	}
	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}


	void Renderer2D::BeginScene(const OrthographicCamera& cam)
	{
		s_Data->flatColor->Bind();
		s_Data->flatColor->UploadUniformMat4("u_ViewProjection", cam.GetViewProjectionMatrix());

		s_Data->texShader->Bind();
		s_Data->texShader->UploadUniformMat4("u_ViewProjection", cam.GetViewProjectionMatrix());
	}
	void Renderer2D::EndScene()
	{

	}
	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ pos.x, pos.y, 0 }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->flatColor->Bind();
		s_Data->flatColor->UploadUniformFloat4("u_Color", color);

		//translation * rotation * scale;
		glm::mat4 transform = glm::translate(glm::mat4(1), pos) * glm::scale(glm::mat4(1), { size.x, size.y, 1 });

		s_Data->flatColor->UploadUniformMat4("u_Transform", transform);

		s_Data->va->Bind();
		RenderCommand::DrawIndexed(s_Data->va);
	}

	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<Texture2D>& tex)
	{
		DrawQuad({ pos.x, pos.y, 0 }, size, tex);
	}
	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<Texture2D>& tex)
	{
		s_Data->texShader->Bind();

		//translation * rotation * scale;
		glm::mat4 transform = glm::translate(glm::mat4(1), pos) * glm::scale(glm::mat4(1), { size.x, size.y, 1 });

		tex->Bind();

		s_Data->texShader->UploadUniformMat4("u_Transform", transform);

		s_Data->va->Bind();
		RenderCommand::DrawIndexed(s_Data->va);
	}
}