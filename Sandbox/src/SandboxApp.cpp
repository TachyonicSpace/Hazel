#include <Hazel.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>



#include "Platform/OpenGl/OpenGLShader.h"


class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("example"), m_Camera(1280.f/720.f), m_SquareColor(1, 0, 1, 1)
	{
		m_VertexArray = Hazel::VertexArray::Create();

		float vertices[3 * 7] =
		{
			-.5f, -.5f, 0.f,	 .8f, .2f, .8f, 1.f,
			.5f, -.5f, 0.f,		 .2f, .3f, .8f, 1.f,
			0.f, .5f, 0.f,		 .8f, .8f, .2f, 1.f
		};

		Hazel::Ref<Hazel::VertexBuffer> vb;
		vb = Hazel::VertexBuffer::Create(vertices, sizeof(vertices));


		Hazel::BufferLayout layout = {
			{Hazel::ShaderDataType::Float3, "position"},
			{Hazel::ShaderDataType::Float4, "color"}
		};

		vb->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vb);


		uint32_t indices[3] = { 0, 1, 2 };
		Hazel::Ref<Hazel::IndexBuffer> ib;
		ib = Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(indices[0]));
		m_VertexArray->AddIndexBuffer(ib);


		float squareVertices[5 * 4] =
		{
			-.5f, -.5f, 0.f, 0.f, 0.f,
			 .5f, -.5f, 0.f, 1.f, 0.f,
			 .5f,  .5f, 0.f, 1.f, 1.f,
			-.5f,  .5f, 0.f, 0.f, 1.f
		};

		m_SquareVertexArray = (Hazel::VertexArray::Create());

		Hazel::BufferLayout squareLayout = {
			{Hazel::ShaderDataType::Float3, "position"},
			{Hazel::ShaderDataType::Float2, "a_TextCoord"}
		};

		Hazel::Ref<Hazel::VertexBuffer> squarevb;
		squarevb = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squarevb->SetLayout(squareLayout);
		m_SquareVertexArray->AddVertexBuffer(squarevb);

		uint32_t squareIndices[6] = { 0, 1, 2,
									  2, 3, 0 };
		Hazel::Ref<Hazel::IndexBuffer> squareib;
		squareib = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(squareIndices[0]));
		m_SquareVertexArray->AddIndexBuffer(squareib);

		std::string vertexSrc = R"(

			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_position;
			out vec4 v_color;

			void main()
			{
				v_position = a_Position;
				v_color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}

		)";


		std::string fragmentSrc = R"(

			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_position;
			in vec4 v_color;

			void main()
			{
				//color = vec4(v_position*0.5 + 0.5, 1.0);
				color = v_color;
			}

		)";
		Hazel::Renderer::GetShaderLibrary()->Add(Hazel::Shader::Create("triangle", vertexSrc, fragmentSrc));


		std::string vertexSrc2 = R"(

			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_position;

			void main()
			{
				v_position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}

		)";


		std::string fragmentSrc2 = R"(

			#version 330 core

			layout(location = 0) out vec4 color;

			uniform vec4 u_Color;

			in vec3 v_position;

			void main()
			{
				color = u_Color;//vec4(0.2, 0.3, 0.8, 1.0);
			}

		)";
		Hazel::Renderer::GetShaderLibrary()->Add(Hazel::Shader::Create("flatColor", vertexSrc2, fragmentSrc2));
		Hazel::Renderer::GetShaderLibrary()->Add(Hazel::Shader::Create("assets/shaders/Texture.glsl"));

		m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

		Hazel::Renderer::GetShaderLibrary()->Get("Texture")->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(Hazel::Renderer::GetShaderLibrary()->Get("Texture"))->UploadUniformInt("u_Color", 0);
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square color", &m_SquareColor.x);
		ImGui::End();
	}

	void OnUpdate(Hazel::Timestep& ts) override
	{
		m_Camera.OnUpdate(ts);

		Hazel::RenderCommand::SetClearColor({ .1f, .1f, .2f, 1 });
		Hazel::RenderCommand::Clear();

		Hazel::Renderer::BeginScene(m_Camera.GetCamera());
		{
			static auto scale = glm::scale(glm::mat4(1), glm::vec3(.1));

			Hazel::Renderer::GetShaderLibrary()->Get("flatColor")->Bind();
			std::dynamic_pointer_cast<Hazel::OpenGLShader>(Hazel::Renderer::GetShaderLibrary()->Get("flatColor"))->UploadUniformFloat4("u_Color", m_SquareColor);

			for(int y = 0; y < 20; y++)
			{
				for (int i = 0; i < 20; i++)
				{
					glm::vec3 pos(i * .11f, y * .11f, 0);
					glm::mat4 transform = glm::translate(glm::mat4(1), pos) * scale;
					Hazel::Renderer::Submit(Hazel::Renderer::GetShaderLibrary()->Get("flatColor"), m_SquareVertexArray, transform);
				}
			}

			m_Texture->Bind();
			Hazel::Renderer::Submit(Hazel::Renderer::GetShaderLibrary()->Get("Texture"), m_SquareVertexArray, glm::scale(glm::mat4(1), glm::vec3(1.5)));
			m_ChernoTexture->Bind();
			Hazel::Renderer::Submit(Hazel::Renderer::GetShaderLibrary()->Get("Texture"), m_SquareVertexArray, glm::scale(glm::mat4(1), glm::vec3(1.5)));


			//triangle rendering
			//Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		}
		Hazel::Renderer::EndScene();

	}

	void OnEvent(Hazel::Event& e) override
	{
		m_Camera.OnEvent(e);
	}


	private:
		Hazel::Ref<Hazel::VertexArray> m_VertexArray;

		Hazel::Ref<Hazel::VertexArray> m_SquareVertexArray;
		glm::vec4 m_SquareColor;

		Hazel::Ref <Hazel::Texture2D> m_Texture, m_ChernoTexture;

		Hazel::OrthographicCameraController m_Camera;
};

class Sandbox2D : public Hazel::Layer
{
public:
	Sandbox2D()
		:Layer("2D sandbox"), m_Camera(1280.f / 720.f)
	{
	}

	~Sandbox2D() = default;

	void OnAttatch()
	{
		float squareVertices[3 * 4] =
		{
			-.5f, -.5f, 0.f,
			 .5f, -.5f, 0.f,
			 .5f,  .5f, 0.f,
			-.5f,  .5f, 0.f
		};

		m_VertexArray = (Hazel::VertexArray::Create());

		Hazel::BufferLayout squareLayout = {
			{Hazel::ShaderDataType::Float3, "position"}
		};

		Hazel::Ref<Hazel::VertexBuffer> squarevb;
		squarevb = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squarevb->SetLayout(squareLayout);
		m_VertexArray->AddVertexBuffer(squarevb);

		uint32_t squareIndices[6] = { 0, 1, 2,
									  2, 3, 0 };
		Hazel::Ref<Hazel::IndexBuffer> squareib;
		squareib = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(squareIndices[0]));
		m_VertexArray->AddIndexBuffer(squareib);

		m_Shader = Hazel::Shader::Create("assets/shaders/FlatColor.glsl");
	}

	void OnDetatch()
	{

	}

	void OnUpdate(Hazel::Timestep& ts)

	{
		m_Camera.OnUpdate(ts);

		Hazel::RenderCommand::SetClearColor({ .1f, .1f, .2f, 1 });
		Hazel::RenderCommand::Clear();

		Hazel::Renderer::BeginScene(m_Camera.GetCamera());
		{
			static auto scale = glm::scale(glm::mat4(1), glm::vec3(1.5));

			m_Shader->Bind();
			std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_Shader)->UploadUniformFloat4("u_Color", m_SquareColor);


			Hazel::Renderer::Submit(m_Shader, m_VertexArray, scale);


			//triangle rendering
			//Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		}
		Hazel::Renderer::EndScene();

	}

	void OnImGuiRender()
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("color", &m_SquareColor.x);
		ImGui::End();
	}

	void OnEvent(Hazel::Event& e)
	{
		m_Camera.OnEvent(e);
	}

private:
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;

	Hazel::Ref<Hazel::Shader> m_Shader;

	Hazel::OrthographicCameraController m_Camera;

	glm::vec4 m_SquareColor = { 1, 0, 1, 1 };
};


class Sandbox : public Hazel::Application {
public:
	Sandbox() {
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {

	}
};

Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox;
}