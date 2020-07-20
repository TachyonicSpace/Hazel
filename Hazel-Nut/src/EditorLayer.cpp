#include "EditorLayer.h"
#include "imgui/imgui.h"

namespace Hazel
{

	

	EditorLayer::EditorLayer()
		:Layer("2D sandbox"), m_Camera(1280.f / 720.f)
	{
	}
	void EditorLayer::OnAttatch()
	{
		HZ_PROFILE_FUNCTION();

		m_checkerboard = Texture2D::Create("assets/textures/Checkerboard.png");

		FramebufferSpecs fbspec;
		fbspec.Width = 900;
		fbspec.Height = 500;
		m_FrameBuffer = Framebuffer::Create(fbspec);
	}
	void EditorLayer::OnDetatch()
	{
		HZ_PROFILE_FUNCTION();

	}
	void EditorLayer::OnUpdate(Timestep& ts)

	{
		HZ_PROFILE_FUNCTION();

		m_Camera.OnUpdate(ts);

		Renderer2D::ResetStats();
		{
			HZ_PROFILE_SCOPE("Renderer::setup");

			m_FrameBuffer->Bind();

			RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
			RenderCommand::Clear();
		}

		{
			HZ_PROFILE_SCOPE("Renderer::draw");

			Renderer2D::BeginScene(m_Camera.GetCamera());

			Renderer2D::DrawQuad({ 0, 0, -.1 }, { 10, 10 }, m_angle, m_checkerboard, 10.f);


			Renderer2D::DrawQuad({ -1, 0 }, { .8, .8 }, {1, 0, 0});
			Renderer2D::DrawQuad({ .5, -.5 }, { .5, .75 }, m_SquareColor);

			for (float y = -5; y <= 5; y += m_Delta)
			{
				for (float x = -5; x <= 5; x += m_Delta)
				{
					Renderer2D::DrawQuad({ x, y, 0 }, { m_Delta*.9, m_Delta*.9 },
						{ (x + 5)/10.f, .5, (x + 5) / 10.f , .7});
				}
			}

		}


		Renderer2D::EndScene();

		m_FrameBuffer->UnBind();
	}
	void EditorLayer::OnImGuiRender()
	{
		if (dockSpaceOpen == true)
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
				ImGui::SetNextWindowPos(viewport->GetWorkPos());
				ImGui::SetNextWindowSize(viewport->GetWorkSize());
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
		}


		ImGui::Begin("Settings");
		ImGui::SliderFloat("angle", &m_angle, 0, 2 * 3.1416);

		auto& stats = Renderer2D::GetStats();

		ImGui::TextColored({ .8, .2, .2, 1 }, "number of draw calls: %d", stats.drawCalls);
		ImGui::TextColored({ .8, .2, .2, 1 }, "number of quads: %d", stats.quadCount);

		static float windowScalar = 1;
		ImGui::SliderFloat("windowSize", &windowScalar, 1, 10);

		if (ImGui::Button("enable dockspace?", { 150, 20 }))
		{
			dockSpaceOpen = !dockSpaceOpen;
			if (dockSpaceOpen)
				ImGui::Begin("tmp to stop from crashing");
			else ImGui::End();
		}
		ImGui::End();

		if (dockSpaceOpen)
			ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("viewPort");
		auto viewPortSize = ImGui::GetContentRegionAvail();

		if (viewPortSize.x != m_ViewPortSize.x || viewPortSize.y != m_ViewPortSize.y) 
		{
			m_FrameBuffer->Resize((uint32_t)viewPortSize.x, (uint32_t)viewPortSize.y);
			m_ViewPortSize = { viewPortSize.x, viewPortSize.y };

			m_Camera.OnResize(viewPortSize.x, viewPortSize.y);
		}
		ImGui::Image((void*)m_FrameBuffer->GetColorAttachmentID(),
			{ m_ViewPortSize.x, m_ViewPortSize.y });
		ImGui::PopStyleVar();
		ImGui::End();
	}
	void EditorLayer::OnEvent(Event& e)
	{
		m_Camera.OnEvent(e);
	}
}