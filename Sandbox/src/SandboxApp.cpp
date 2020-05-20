#include <Hazel.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>


class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("example"), cam(-1.6f, 1.6f, -0.9f, .9f)
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
		m_Shader.reset(new Hazel::Shader(vertexSrc, fragmentSrc));


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

			in vec3 v_position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}

		)";
		m_squareShader.reset(new Hazel::Shader(vertexSrc2, fragmentSrc2));

		cam.SetPosition({ 0, 0, 0 });
		cam.SetRotation(0);
	}

	void OnImGuiRender() override
	{
	}

	void OnUpdate(Hazel::Timestep& ts) override
	{
		HZ_CORE_TRACE("Delta time:{0} seconds, [{1} milliseconds]\n", ts.GetSeconds(), ts.GetMilliseconds());

		auto& pos = cam.GetPos();
		float moveSpeed = 1, rotateSpeed = 180;
		if (Hazel::Input::IsKeyPressed(HZ_KEY_W))
			cam.SetPosition({ pos.x, pos.y - moveSpeed * ts, pos.z });

		if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
			cam.SetPosition({ pos.x + moveSpeed * ts, pos.y, pos.z });

		if (Hazel::Input::IsKeyPressed(HZ_KEY_S))
			cam.SetPosition({ pos.x, pos.y + moveSpeed * ts, pos.z });

		if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
			cam.SetPosition({ pos.x - moveSpeed * ts, pos.y, pos.z });



		if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
			cam.SetRotation(cam.GetRotation() + rotateSpeed * ts);

		if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
			cam.SetRotation(cam.GetRotation() - rotateSpeed * ts);



		Hazel::RenderCommand::SetClearColor({ .1f, .1f, .2f, 1 });
		Hazel::RenderCommand::Clear();

		Hazel::Renderer::BeginScene(cam);
		{
			static auto scale = glm::scale(glm::mat4(1), glm::vec3(.1));

			for(int y = 0; y < 20; y++)
			{
				for (int i = 0; i < 20; i++)
				{
					glm::vec3 pos(i * .11f, y * .11f, 0);
					glm::mat4 transform = glm::translate(glm::mat4(1), pos) * scale;
					Hazel::Renderer::Submit(m_squareShader, m_SquareVertexArray, transform);
				}
			}

			Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		}
		Hazel::Renderer::EndScene();

	}

	void OnEvent(Hazel::Event& event) override
	{
		
	}


	private:
		std::shared_ptr<Hazel::Shader> m_Shader;
		std::shared_ptr<Hazel::VertexArray> m_VertexArray;

		std::shared_ptr<Hazel::Shader> m_squareShader;
		std::shared_ptr<Hazel::VertexArray> m_SquareVertexArray;

		Hazel::OrthographicCamera cam;
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