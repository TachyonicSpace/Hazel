#include <Hazel.h>

#include "imgui/imgui.h"


std::shared_ptr<Hazel::Shader> m_Shader;
std::shared_ptr<Hazel::VertexArray> m_VertexArray;

std::shared_ptr<Hazel::Shader> m_squareShader;
std::shared_ptr<Hazel::VertexArray> m_SquareVertexArray;

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("example")
	{
		m_VertexArray.reset(Hazel::VertexArray::Create());

		float vertices[3 * 7] =
		{
			-.5f, -.5f, 0.f,	 .8f, .2f, .8f, 1.f,
			.5f, -.5f, 0.f,		 .2f, .3f, .8f, 1.f,
			0.f, .5f, 0.f,		 .8f, .8f, .2f, 1.f
		};

		std::shared_ptr<Hazel::VertexBuffer> vb;
		vb.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));


		Hazel::BufferLayout layout = {
			{Hazel::ShaderDataType::Float3, "position"},
			{Hazel::ShaderDataType::Float4, "color"}
		};

		vb->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vb);


		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Hazel::IndexBuffer> ib;
		ib.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(indices[0])));
		m_VertexArray->AddIndexBuffer(ib);


		float squareVertices[3 * 4] =
		{
			-.5f, -.5f, 0.f,
			 .5f, -.5f, 0.f,
			 .5f,  .5f, 0.f,
			-.5f,  .5f, 0.f
		};

		m_SquareVertexArray.reset((Hazel::VertexArray::Create()));

		Hazel::BufferLayout squareLayout = {
			{Hazel::ShaderDataType::Float3, "position"}
		};

		std::shared_ptr<Hazel::VertexBuffer> squarevb;
		squarevb.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squarevb->SetLayout(squareLayout);
		m_SquareVertexArray->AddVertexBuffer(squarevb);

		uint32_t squareIndices[6] = { 0, 1, 2,
									  2, 3, 0 };
		std::shared_ptr<Hazel::IndexBuffer> squareib;
		squareib.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(squareIndices[0])));
		m_SquareVertexArray->AddIndexBuffer(squareib);

		std::string vertexSrc = R"(

			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_position;
			out vec4 v_color;

			void main()
			{
				v_position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
				v_color = a_Color;
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
		m_Shader.reset(new Hazel::Shader(vertexSrc, fragmentSrc));


		std::string vertexSrc2 = R"(

			#version 330 core

			layout(location = 0) in vec3 a_Position;

			out vec3 v_position;

			void main()
			{
				v_position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}

		)";


		std::string fragmentSrc2 = R"(

			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}

		)";
		m_squareShader.reset(new Hazel::Shader(vertexSrc2, fragmentSrc2));
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("name");
		ImGui::Text("hello mortals");
		ImGui::End();
	}

	void OnUpdate() override
	{
		Hazel::Renderer::BeginScene();
		{

			m_squareShader->Bind();
			Hazel::Renderer::Submit(m_SquareVertexArray);

			m_Shader->Bind();
			Hazel::Renderer::Submit(m_VertexArray);

		}
		Hazel::Renderer::EndScene();



		if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB)) {
			HZ_INFO("tab is pressed");
		}
	}

	void OnEvent(Hazel::Event& event) override
	{
		if (event.GetEventType() == Hazel::EventType::KeyPressed)
		{
			Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;
			HZ_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
};

class Sandbox : public Hazel::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		//PushOverlay(new Hazel::ImGuiLayer());
	}

	~Sandbox() {

	}
};

Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox;
}