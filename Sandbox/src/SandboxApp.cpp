#define HZ_MAIN
#include <Hazel.h>

#include "imgui/imgui.h"

#include "ParticleSystem.h"

static const uint32_t s_MapWidth = 24;

static const char* s_MapTiles =
"000000000000000000000000"
"0000000dddd0000000000000"
"0000dddddddddddd00000000"
"000dd00ddddddddddddd0000"
"00dd000ddddddddddddddd00"
"0000ddddddddddddddddd000"
"000000dddddddddddddd0000"
"000000000dddddddd0000000"
"000000000000000000000000"
;



//class ExampleLayer : public Hazel::Layer
//{
//public:
//	ExampleLayer()
//		: Layer("example"), m_Camera(1280.f/720.f), m_SquareColor(1, 0, 1, 1)
//	{
//		m_VertexArray = Hazel::VertexArray::Create();
//
//		float vertices[3 * 7] =
//		{
//			-.5f, -.5f, 0.f,	 .8f, .2f, .8f, 1.f,
//			.5f, -.5f, 0.f,		 .2f, .3f, .8f, 1.f,
//			0.f, .5f, 0.f,		 .8f, .8f, .2f, 1.f
//		};
//
//		Hazel::Ref<Hazel::VertexBuffer> vb;
//		vb = Hazel::VertexBuffer::Create(vertices, sizeof(vertices));
//
//
//		Hazel::BufferLayout layout = {
//			{Hazel::ShaderDataType::Float3, "position"},
//			{Hazel::ShaderDataType::Float4, "color"}
//		};
//
//		vb->SetLayout(layout);
//		m_VertexArray->AddVertexBuffer(vb);
//
//
//		uint32_t indices[3] = { 0, 1, 2 };
//		Hazel::Ref<Hazel::IndexBuffer> ib;
//		ib = Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(indices[0]));
//		m_VertexArray->AddIndexBuffer(ib);
//
//
//		float squareVertices[5 * 4] =
//		{
//			-.5f, -.5f, 0.f, 0.f, 0.f,
//			 .5f, -.5f, 0.f, 1.f, 0.f,
//			 .5f,  .5f, 0.f, 1.f, 1.f,
//			-.5f,  .5f, 0.f, 0.f, 1.f
//		};
//
//		m_SquareVertexArray = (Hazel::VertexArray::Create());
//
//		Hazel::BufferLayout squareLayout = {
//			{Hazel::ShaderDataType::Float3, "position"},
//			{Hazel::ShaderDataType::Float2, "a_TextCoord"}
//		};
//
//		Hazel::Ref<Hazel::VertexBuffer> squarevb;
//		squarevb = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
//		squarevb->SetLayout(squareLayout);
//		m_SquareVertexArray->AddVertexBuffer(squarevb);
//
//		uint32_t squareIndices[6] = { 0, 1, 2,
//									  2, 3, 0 };
//		Hazel::Ref<Hazel::IndexBuffer> squareib;
//		squareib = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(squareIndices[0]));
//		m_SquareVertexArray->AddIndexBuffer(squareib);
//
//		std::string vertexSrc = R"(
//
//			#version 330 core
//
//			layout(location = 0) in vec3 a_Position;
//			layout(location = 1) in vec4 a_Color;
//
//			uniform mat4 u_ViewProjection;
//			uniform mat4 u_Transform;
//
//			out vec3 v_position;
//			out vec4 v_color;
//
//			void main()
//			{
//				v_position = a_Position;
//				v_color = a_Color;
//				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
//			}
//
//		)";
//
//
//		std::string fragmentSrc = R"(
//
//			#version 330 core
//
//			layout(location = 0) out vec4 color;
//
//			in vec3 v_position;
//			in vec4 v_color;
//
//			void main()
//			{
//				//color = vec4(v_position*0.5 + 0.5, 1.0);
//				color = v_color;
//			}
//
//		)";
//		Hazel::Renderer::GetShaderLibrary()->Add(Hazel::Shader::Create("triangle", vertexSrc, fragmentSrc));
//
//
//		std::string vertexSrc2 = R"(
//
//			#version 330 core
//
//			layout(location = 0) in vec3 a_Position;
//
//			uniform mat4 u_ViewProjection;
//			uniform mat4 u_Transform;
//
//			out vec3 v_position;
//
//			void main()
//			{
//				v_position = a_Position;
//				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
//			}
//
//		)";
//
//
//		std::string fragmentSrc2 = R"(
//
//			#version 330 core
//
//			layout(location = 0) out vec4 color;
//
//			uniform vec4 u_Color;
//
//			in vec3 v_position;
//
//			void main()
//			{
//				color = u_Color;//vec4(0.2, 0.3, 0.8, 1.0);
//			}
//
//		)";
//		Hazel::Renderer::GetShaderLibrary()->Add(Hazel::Shader::Create("flatColor", vertexSrc2, fragmentSrc2));
//		Hazel::Renderer::GetShaderLibrary()->Add(Hazel::Shader::Create("assets/shaders/Texture.glsl"));
//
//		m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
//		m_ChernoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");
//
//		Hazel::Renderer::GetShaderLibrary()->Get("Texture")->Bind();
//		std::dynamic_pointer_cast<Hazel::OpenGLShader>(Hazel::Renderer::GetShaderLibrary()->Get("Texture"))->UploadUniformInt("u_Color", 0);
//	}
//
//	void OnImGuiRender() override
//	{
//		ImGui::Begin("Settings");
//		ImGui::ColorEdit3("Square color", &m_SquareColor.x);
//		ImGui::End();
//	}
//
//	void OnUpdate(Hazel::Timestep& ts) override
//	{
//		m_Camera.OnUpdate(ts);
//
//		Hazel::RenderCommand::SetClearColor({ .1f, .1f, .2f, 1 });
//		Hazel::RenderCommand::Clear();
//
//		Hazel::Renderer::BeginScene(m_Camera.GetCamera());
//		{
//			static auto scale = glm::scale(glm::mat4(1), glm::vec3(.1));
//
//			Hazel::Renderer::GetShaderLibrary()->Get("flatColor")->Bind();
//			std::dynamic_pointer_cast<Hazel::OpenGLShader>(Hazel::Renderer::GetShaderLibrary()->Get("flatColor"))->UploadUniformFloat4("u_Color", m_SquareColor);
//
//			for(int y = 0; y < 20; y++)
//			{
//				for (int i = 0; i < 20; i++)
//				{
//					glm::vec3 pos(i * .11f, y * .11f, 0);
//					glm::mat4 transform = glm::translate(glm::mat4(1), pos) * scale;
//					Hazel::Renderer::Submit(Hazel::Renderer::GetShaderLibrary()->Get("flatColor"), m_SquareVertexArray, transform);
//				}
//			}
//
//			m_Texture->Bind();
//			Hazel::Renderer::Submit(Hazel::Renderer::GetShaderLibrary()->Get("Texture"), m_SquareVertexArray, glm::scale(glm::mat4(1), glm::vec3(1.5)));
//			m_ChernoTexture->Bind();
//			Hazel::Renderer::Submit(Hazel::Renderer::GetShaderLibrary()->Get("Texture"), m_SquareVertexArray, glm::scale(glm::mat4(1), glm::vec3(1.5)));
//
//
//			//triangle rendering
//			//Hazel::Renderer::Submit(m_Shader, m_VertexArray);
//
//		}
//		Hazel::Renderer::EndScene();
//
//	}
//
//	void OnEvent(Hazel::Event& e) override
//	{
//		m_Camera.OnEvent(e);
//	}
//
//
//	private:
//		Hazel::Ref<Hazel::VertexArray> m_VertexArray;
//
//		Hazel::Ref<Hazel::VertexArray> m_SquareVertexArray;
//		glm::vec4 m_SquareColor;
//
//		Hazel::Ref <Hazel::Texture2D> m_Texture, m_ChernoTexture;
//
//		Hazel::OrthographicCameraController m_Camera;
//};

class Sandbox2D : public Hazel::Layer
{
public:
	Sandbox2D()
		:Layer("2D sandbox"), m_Camera(1280.f / 720.f)
	{
	}

	~Sandbox2D() = default;

	void OnAttach()
	{
		HZ_PROFILE_FUNCTION();

		m_checkerboard = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		m_SpriteSheet = Hazel::Texture2D::Create("assets/game/RPGpack_sheet_2X.png");

		Hazel::FramebufferSpecs fbspec;
		fbspec.Width = 900;
		fbspec.Height = 500;

		m_TextureMap['d'] = { 6, 11 };
		m_TextureMap['0'] = { 11, 11 };

		m_Particle.ColorBegin = { 254, 212, 123, 1 };
		m_Particle.ColorEnd = { 254, 109, 41, 1 };
		m_Particle.SizeBegin = .5, m_Particle.SizeVariation = .3, m_Particle.SizeEnd = 0;
		m_Particle.LifeTime = 1;
		m_Particle.Velocity = { 0, 0 };
		m_Particle.VelocityVariation = { 3, 1 };
		m_Particle.Position = { 0, 0 };
	}

	void OnDetach()
	{
		HZ_PROFILE_FUNCTION();

	}

	void OnUpdate(Hazel::Timestep& ts)

	{
		HZ_PROFILE_FUNCTION();

		m_Camera.OnUpdate(ts);

		Hazel::Renderer2D::ResetStats();
		{
			HZ_PROFILE_SCOPE("Renderer::setup");
			
			Hazel::RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
			Hazel::RenderCommand::Clear();
		}

		/*{
			HZ_PROFILE_SCOPE("Renderer::draw");

			Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());

			Hazel::Renderer2D::DrawQuad({ 0, 0, -.1 }, { 10, 10 }, m_angle, m_checkerboard, 10.f);


			Hazel::Renderer2D::DrawQuad({ -1, 0 }, { .8, .8 }, {1, 0, 0});
			Hazel::Renderer2D::DrawQuad({ .5, -.5 }, { .5, .75 }, m_SquareColor);

			for (float y = -5; y <= 5; y += delta)
			{
				for (float x = -5; x <= 5; x += delta)
				{
					Hazel::Renderer2D::DrawQuad({ x, y, 0 }, { delta*.9, delta*.9 },
						{ (x + 5)/10.f, .5, (x + 5) / 10.f , .7});
				}
			}

		}*/

		Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());
		m_SpriteSheet->subTexture(texCoordX, texCoordY);
		Hazel::Renderer2D::DrawQuad({ 0, 0 }, { 1, 1 }, m_SpriteSheet);

		{
			HZ_PROFILE_SCOPE("End Scene");
			Hazel::Renderer2D::EndScene();
		}

		Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());

		uint32_t mapHeight = strlen(s_MapTiles) / s_MapWidth;
		for (uint32_t y = 0; y < mapHeight; y++)
		{
			for (uint32_t x = 0; x < s_MapWidth; x++)
			{
				char tile = s_MapTiles[x + y * s_MapWidth];
				glm::vec2 tilePos = { x - s_MapWidth / 2.0, mapHeight - y - mapHeight / 2.0 };
				glm::vec2 tileSize = { 1, 1 };

				if (m_TextureMap.find(tile) != m_TextureMap.end())
				{
					auto subTextureCoords = m_TextureMap[tile];
					m_SpriteSheet->subTexture(subTextureCoords.x, subTextureCoords.y);
					Hazel::Renderer2D::DrawQuad(tilePos, tileSize, m_SpriteSheet);

				}
				else
					Hazel::Renderer2D::DrawQuad(tilePos, tileSize, { 1, 0, 1 });
			}
		}


		Hazel::Renderer2D::EndScene();

		/*if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Hazel::Input::GetMousePos();
			auto width = Hazel::Application::Get().GetWindow().GetWidth();
			auto height = Hazel::Application::Get().GetWindow().GetHeight();

			auto bounds = m_Camera.GetBounds();
			auto pos = m_Camera.GetCamera().GetPos();
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			m_Particle.Position = { x + pos.x, y + pos.y };
			for (int i = 0; i < 5; i++)
				m_ParticleSystem.Emit(m_Particle);
		}

		m_ParticleSystem.OnUpdate(ts);
		m_ParticleSystem.OnRender(m_Camera.GetCamera());*/
	}

	void OnImGuiRender()
	{
		
		ImGui::Begin("Settings");
		ImGui::SliderFloat("angle", &m_angle, 0, 2 * 3.1416);
		ImGui::SliderInt("texture x coord", &texCoordX, -1, m_SpriteSheet->GetWidth() / 128);
		ImGui::SliderInt("texture y coord", &texCoordY, -1, m_SpriteSheet->GetHeight() / 128);

		auto& stats = Hazel::Renderer2D::GetStats();

		ImGui::TextColored({ .8, .2, .2, 1 }, "number of draw calls: %d", stats.drawCalls);
		ImGui::TextColored({ .8, .2, .2, 1 }, "number of quads: %d", stats.quadCount);

	}

	void OnEvent(Hazel::Event& e)
	{
		m_Camera.OnEvent(e);
	}

private:
	Hazel::OrthographicCameraController m_Camera;

	Hazel::Ref<Hazel::VertexArray> m_VertexArray;
	Hazel::Ref<Hazel::Shader> m_Shader;

	Hazel::Ref<Hazel::Texture2D> m_checkerboard;
	Hazel::Ref<Hazel::Texture2D> m_SpriteSheet;

	glm::vec4 m_SquareColor = { 1, 0, 1, 1 };

	float m_angle = 0;
	int texCoordX = -1, texCoordY = -1;

	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;

	std::unordered_map<char, glm::vec2> m_TextureMap;

	bool dockSpaceOpen = false;

};


class Sandbox : public Hazel::Application {
public:
	Sandbox() {
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {

	}
};

Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox;
}