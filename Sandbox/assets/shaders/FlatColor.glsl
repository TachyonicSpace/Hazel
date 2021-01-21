#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
	color = u_Color;
}



//sandboz.app
#define HZ_MAIN
#define _USE_MATH_DEFINES
#include <Hazel.h>
#include "Rocket.h"
#include <random>

#include "imgui/imgui.h"



#define WindowWidth 960.0f
#define WindowHeight 540.0f
float rocketW = .065;
float rocketH = .025;
float popsize = 100;
vec4 obstacle(0, 0, 1, .2);

int testing = -1;


bool restart = false;

struct population {
	std::vector<Rocket> rockets;
	std::vector<Rocket> matingPool;

	float prevPopSize;

	population(float width, float height) {
		Rocket::count = 0;
		for (int i = 0; i < popsize; i++) {
			rockets.push_back(*(new Rocket(width, height)));
		}
		prevPopSize = popsize;
	}

	void pos(glm::vec2 pos) {
		for (Rocket r : rockets) {
			r.Pos = pos;
		}
	}

	float eval() {
		float bestFit = rockets[0].fitness(&obstacle);
		for (int i = 0; i < rockets.size(); i++) {
			bestFit = __min(rockets[i].fitness(&obstacle), bestFit);
		}
		matingPool.resize(0);
		//matingPool.clear();
		//matingPool.shrink_to_fit();
		for (int i = 0; i < rockets.size(); i++) {
			float fitness = rockets[i].Fitness;
			auto n = clamp(fitness, 1.f, 1000.f);
			for (int j = 0; j < n; j++) {
				matingPool.push_back(rockets[i]);
			}
		}
		matingPool.shrink_to_fit();
		return bestFit;
	}

	void selection() {
		/*
		population* newpop = new population();*/
		if (matingPool.size() <= 0) {
			matingPool = rockets;
			std::cout << "Error: MatingPool empty, making the mating pool equal to the population\n\n";
		}
		rockets.resize(popsize);
		for (int i = 0; i < rockets.size(); i++) {
			auto index = Rand((matingPool.size() - 1), 0);
			auto partnerA = (matingPool[index]);

			index = Rand((matingPool.size() - 1), 0);
			auto partnerB = (matingPool[index]);

			auto child = partnerA.crossover(partnerB);

			this->rockets[i] = child;
		}
		prevPopSize = popsize;
	}


};

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
	Hazel::OrthographicCameraController m_Camera;

	population pop = population(rocketW, rocketH);

	float angle = 0;

	int Rolls = 10000;
	int stars = 500;
	float mean = 5.0, sd = 2.0;
	float testValue = -.55;
	std::default_random_engine gen;
	std::normal_distribution<double>* dist = new std::normal_distribution<double>(mean, 1 / sd);

	bool m_ViewPortFocused, m_ViewPortHovered;
	glm::vec2 m_ViewPortSize;
	glm::vec2 m_ViewportBounds[2];

	Hazel::Ref<Hazel::Framebuffer> m_FrameBuffer;

public:
	Sandbox2D()
		:Layer("2D sandbox"), m_Camera(Hazel::Application::Get().GetWindow().GetAspectRatio())
	{
	}

	~Sandbox2D() = default;

	void OnAttach()
	{
		HZ_PROFILE_FUNCTION();

		Hazel::FramebufferSpecs fbspec;
		fbspec.Width = 1280;
		fbspec.Height = 720;
		m_FrameBuffer = Hazel::Framebuffer::Create(fbspec);

		//Hazel::Application::Get().GetWindow().SetVSync(false);
	}
	void OnDetach()
	{
		HZ_PROFILE_FUNCTION();

	}

	void OnUpdate(Hazel::Timestep& ts)
	{
		{
			if (Hazel::FramebufferSpecs spec = m_FrameBuffer->GetSpecs();
				m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && // zero sized framebuffer is invalid
				(spec.Width != m_ViewPortSize.x || spec.Height != m_ViewPortSize.y))
			{
				m_FrameBuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);

				m_Camera.OnResize(m_ViewPortSize.x, m_ViewPortSize.y);
			}

			if (m_ViewPortFocused)
			{

				m_Camera.OnUpdate(ts);

			}
			Hazel::Renderer2D::ResetStats();



			m_FrameBuffer->Bind();

			Hazel::RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
			Hazel::RenderCommand::Clear();

			m_FrameBuffer->Bind();
		}

		if (restart)
		{
			pop = *(new population(rocketW, rocketH));
			restart = false;
		}
		Hazel::Renderer2D::ResetStats();
		Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());

		bool notFinished = false;
		for (int i = 0; i < pop.rockets.size(); i++)
		{

			Rocket& r = pop.rockets[i];
			Hazel::Renderer2D::DrawQuad(r.pos(), { rocketW, rocketH }, r.angle, r.col);

			notFinished |= r.update(&obstacle);

		}

		if (!notFinished)
			goto Finished;



		{
			Rocket target;
			Hazel::Renderer2D::DrawQuad(target.target, { Rocket::targetSize, Rocket::targetSize }, { 0, 1, 1, .8 });
		}

		{
			Rocket obstacle;
			Hazel::Renderer2D::DrawQuad({ 0, 0, -.8 }, { 1, .2 }, { 1, 0, .8 });
		}

		Rocket::count++;
		if (Rocket::count >= DNA::lifespan) {
		Finished:
			Rocket::count = 0;
			//Rocket::targetSize *= (Rocket::targetSize < .005) ? 1 : .9f;
			pop.eval();
			pop.selection();
		}


		Hazel::Renderer2D::EndScene();

		m_FrameBuffer->UnBind();

	}
	void OnImGuiRender()
	{
		//docking in imgui
		{
			{
				static bool opt_fullscreen_persistant = true;
				bool opt_fullscreen = opt_fullscreen_persistant;
				static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

				// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
				// because it would be confusing to have two docking targets within each others.
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
				if (opt_fullscreen)
				{
					ImGuiViewport* viewport = ImGui::GetMainViewport();
					ImGui::SetNextWindowPos(viewport->Pos);
					ImGui::SetNextWindowSize(viewport->Size);
					ImGui::SetNextWindowViewport(viewport->ID);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
					window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
					window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
				}

				// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
				// and handle the pass-thru hole, so we ask Begin() to not render a background.
				if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
					window_flags |= ImGuiWindowFlags_NoBackground;

				// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
				// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
				// all active windows docked into it will lose their parent and become undocked.
				// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
				// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				bool dockSpaceOpen = true;
				ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
				ImGui::PopStyleVar();

				if (opt_fullscreen)
					ImGui::PopStyleVar(2);

				// DockSpace
				ImGuiIO& io = ImGui::GetIO();
				ImGuiStyle& style = ImGui::GetStyle();
				float minWinSizeX = style.WindowMinSize.x;
				style.WindowMinSize.x = 370.0f;
				if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
				{
					ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
					ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
				}

				style.WindowMinSize.x = minWinSizeX;

			}
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows,
					// which we can't undo at the moment without finer window depth/z control.
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

					if (ImGui::MenuItem("Exit"))
						Hazel::Application::Get().Close();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

		}

		//settings
		{
			ImGui::Begin("Settings");

			#if 1
			ImGui::Checkbox("restart", &restart);

			ImGui::SliderInt("index", &testing, -1, pop.rockets.size() - 1);
			ImGui::SliderFloat("popsize", &popsize, 1, 1000);

			if (testing == -1)
			{
				static auto& window = Hazel::Application::Get().GetWindow();
				static Rocket rocket = pop.rockets[0];
				rocket.Pos = { Hazel::Input::GetMouseX() / window.GetWidth() * 2.f * window.GetAspectRatio() - window.GetAspectRatio(), -(Hazel::Input::GetMouseY() / window.GetHeight() * 2.f - 1) };
				ImGui::Text("pos: %f, %f", rocket.Pos.x, rocket.Pos.y);
				ImGui::Text("vel: x~%f, y~%f", rocket.vel.x, rocket.vel.y);
				ImGui::Text("acc: x~%f, y~%f", rocket.acc.x, rocket.acc.y);
				ImGui::ColorEdit4("color:", rocket.col.GetRGBAPointer());
				ImGui::SliderFloat("mutationRate", &Rocket::mutationRate, 0, 1);
				ImGui::SliderFloat("mutationPow", &Rocket::mutationpow, -1, 2);
				rocket.angle += .5f;
				rocket.angle = fmod(rocket.angle, 3.141592f * 2.0f);
				ImGui::Text("fitness: %f", rocket.fitness(&obstacle));

				Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());
				Hazel::Renderer2D::DrawQuad(rocket.pos(), { rocketW, rocketH }, rocket.angle, rocket.col);
				Hazel::Renderer2D::EndScene();
			}
			else
			{
				if (testing > popsize - 1)
					testing = popsize - 1;
				Rocket& rocket = pop.rockets[testing];
				ImGui::Text("stats of rocket %p:", &rocket);
				ImGui::SliderFloat2("pos: ", &rocket.Pos.x, -2, 2);
				ImGui::Text("vel: x~%f, y~%f", rocket.vel.x, rocket.vel.y);
				ImGui::Text("acc: x~%f, y~%f", rocket.acc.x, rocket.acc.y);
				ImGui::ColorEdit4("color:", rocket.col.GetRGBAPointer());
				ImGui::SliderFloat("mutationRate", &Rocket::mutationRate, 0, 1);
				ImGui::SliderFloat("mutationPow", &Rocket::mutationpow, -1, 2);
				ImGui::Text("angle: %f", rocket.angle);
				ImGui::Text("fitness: %f", rocket.fitness(&obstacle));
			}
			ImGui::Separator();

			auto& stats = Hazel::Renderer2D::GetStats();

			ImGui::TextColored({ .8, .2, .2, 1 }, "number of draw calls: %d", stats.drawCalls);
			ImGui::TextColored({ .8, .2, .2, 1 }, "number of quads: %d", stats.quadCount);

			#else

			constexpr int range = 50;


			ImGui::SliderFloat("mean", &mean, 0, range);
			ImGui::SliderFloat("standard deviation", &sd, .2, range);
			if (dist->mean() != mean || dist->sigma() != sd)
			{
				free(dist);
				dist = new std::normal_distribution<double>(mean, 1 / sd);
			}


			std::stringstream normal;
			int list[range] = {};

			for (int i = 0; i < Rolls; i++)
			{
				double number = (*dist)(gen);
				if ((number >= 0) && (number < range))
					++list[int(number)];
			}
			normal << "normal dist (" << mean << ", " << sd << "):\n";

			for (int i = 0; i < range; i++)
			{
				normal << i << '-' << (i + 1) << ": ";
				normal << std::string(list[i] * stars / Rolls, '*') << "\n";
			}

			//auto zvalue = (mean - testValue) / sd;
			//std::string probability = std::to_string(std::erfc(-zvalue * M_SQRT1_2)/2.0);
			ImGui::SliderFloat("probability", &testValue, 0, range);
			//ImGui::TextColored(ImVec4(0, 1, 1, 1), probability.c_str());
			ImGui::TextColored(ImVec4(1, 0, 1, 1), normal.str().c_str());
			#endif

			ImGui::End();
		}

		//viewport, main scene
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			ImGui::Begin("viewPort");


			auto viewportOffset = ImGui::GetCursorPos(); //includes tab bar

			m_ViewPortFocused = ImGui::IsWindowFocused();
			m_ViewPortHovered = ImGui::IsWindowHovered();
			Hazel::Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused && !m_ViewPortHovered);

			auto viewPortSize = ImGui::GetContentRegionAvail();
			m_ViewPortSize = { viewPortSize.x, viewPortSize.y };


			ImGui::Image((void*)(uint64_t)m_FrameBuffer->GetColorAttachmentID(), { m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });


			auto windowSize = ImGui::GetWindowSize();
			ImVec2 minBound = ImGui::GetWindowPos();
			minBound.x += viewportOffset.x;
			minBound.y += viewportOffset.y;

			ImVec2 maxBound = { minBound.x + windowSize.x - viewportOffset.x, minBound.y + windowSize.y - viewportOffset.y };
			m_ViewportBounds[0] = { minBound.x, minBound.y };
			m_ViewportBounds[1] = { maxBound.x, maxBound.y };

			Hazel::Input::SetViewportOffset(minBound.x, minBound.y);


			ImGui::End();
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}




	void OnEvent(Hazel::Event& e)
	{
		m_Camera.OnEvent(e);
	}


};


class Sandbox : public Hazel::Application {
public:
	Sandbox()
		:Application({ "HazelNut editor" }) {//, 900, 500}) {
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {

	}
};

Hazel::Application* Hazel::CreateApplication(std::string str) {
	return new Sandbox;
}