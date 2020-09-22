#include "EditorLayer.h"
//#include "Hazel/ImGui/ImGui.h"
#include "imgui/imgui.h"


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{



	EditorLayer::EditorLayer()
		:Layer("2D sandbox"), m_Camera(1280.f / 720.f, true)
	{
	}
	void EditorLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();

		m_checkerboard = Texture2D::Create("assets/textures/Checkerboard.png");

		FramebufferSpecs fbspec;
		fbspec.Width = 900;
		fbspec.Height = 500;
		m_FrameBuffer = Framebuffer::Create(fbspec);


		m_Scene = NewRef<Scene>();

		auto square = m_Scene->CreateEntity();

		//square.AddComponent<Transform>();
		square.AddComponent<Component::SpriteRenderer>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		m_SquareEntity = square;

		m_CameraEntity = m_Scene->CreateEntity();
		m_CameraEntity.AddComponent<Component::Cameras>();

		m_SecondCamera = m_Scene->CreateEntity();
		m_SecondCamera.AddComponent<Component::Cameras>().Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate()
			{
			}

			void OnDestroy()
			{
			}

			void OnUpdate(Timestep ts) override
			{
				auto& trans = GetComponent<Component::Transform>().transform;
				float moveSpeed = 5;
				float radianSpeed = .2;

				if (Input::IsKeyPressed(HZ_KEY_A))
				{
					trans[3][0] -= moveSpeed * ts;
				}

				else if (Input::IsKeyPressed(HZ_KEY_D))
				{
					trans[3][0] += moveSpeed * ts;
				}
				if (Input::IsKeyPressed(HZ_KEY_W))
				{
					trans[3][1] += moveSpeed * ts;
				}
				else if (Input::IsKeyPressed(HZ_KEY_S))
				{
					trans[3][1] -= moveSpeed * ts;
				}
			}

		};

		m_CameraEntity.AddComponent<Component::NativeScript>().Bind<CameraController>();
	}
	void EditorLayer::OnDetach()
	{
		HZ_PROFILE_FUNCTION();

	}
	void EditorLayer::OnUpdate(Timestep& ts)

	{
		HZ_PROFILE_FUNCTION();

		if (FramebufferSpecs spec = m_FrameBuffer->GetSpecs();
			m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewPortSize.x || spec.Height != m_ViewPortSize.y))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_Camera.OnResize(m_ViewPortSize.x, m_ViewPortSize.y);

			m_Scene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		}

		if (m_ViewPortFocused)
			m_Camera.OnUpdate(ts);


		m_FrameBuffer->Bind();

		RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
		RenderCommand::Clear();


		m_Scene->OnUpdate(ts);

		m_FrameBuffer->UnBind();
	}
	void EditorLayer::OnImGuiRender()
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
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}



		ImGui::Begin("Settings");
		ImGui::SliderFloat("angle", &m_angle, 0, 2 * 3.1416f);

		auto& stats = Renderer2D::GetStats();

		ImGui::TextColored({ .8f, .2f, .2f, 1.f }, "frame buffer width: %d", m_FrameBuffer->GetSpecs().Width);
		ImGui::TextColored({ .8f, .2f, .2f, 1.f }, "framebuffer height: %d", m_FrameBuffer->GetSpecs().Height);

		const auto& squareColor = m_SquareEntity.GetComponent<Component::SpriteRenderer>().color.GetRGBAPointer();
		ImGui::ColorEdit4("Square Color", squareColor);

		ImGui::Separator();

		ImGui::DragFloat3("Camera Transform",
			glm::value_ptr(m_CameraEntity.GetComponent<Component::Transform>().transform[3]));

		if (ImGui::Checkbox("Camera A", &m_PrimaryCamera))
		{
			m_CameraEntity.GetComponent<Component::Cameras>().Primary = m_PrimaryCamera;
			m_SecondCamera.GetComponent<Component::Cameras>().Primary = !m_PrimaryCamera;
		}
		{
			auto& camera = m_SecondCamera.GetComponent<Component::Cameras>().camera;
			float orthoSize = camera.GetOrthographicSize();
			if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize))
				camera.SetOrthographicSize(orthoSize);
		}


		ImGui::End();



		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("viewPort");

		m_ViewPortFocused = ImGui::IsWindowFocused();
		m_ViewPortHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused || !m_ViewPortHovered);

		auto viewPortSize = ImGui::GetContentRegionAvail();
		m_ViewPortSize = { viewPortSize.x, viewPortSize.y };


		ImGui::Image((void*)m_FrameBuffer->GetColorAttachmentID(),
			{ m_ViewPortSize.x, m_ViewPortSize.y });
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::End();

	}
	void EditorLayer::OnEvent(Event& e)
	{
		m_Camera.OnEvent(e);
	}
}