#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Hazel/Scene/SceneSerializer.h"

#include "Hazel/Utils/PlatformUtils.h"

#include "ImGuizmo.h"
#include <Hazel/Math/Math.h>


namespace Hazel
{
	extern const std::filesystem::path g_AssetPath;

	EditorLayer* EditorLayer::m_MainEditorLayer = nullptr;

	EditorLayer::EditorLayer()
		:Layer("2D sandbox"), m_Camera(1280.f / 720.f, true), m_ViewportBounds()
	{
	}
	void EditorLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();

		m_checkerboard = Texture2D::Create("assets/textures/Checkerboard.png");

		fbspec.Attatchments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbspec.Width = 1280;
		fbspec.Height = 720;
		m_FrameBuffer = Framebuffer::Create(fbspec);

		m_Scene = NewRef<Scene>();

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			std::string sceneFilePath = commandLineArgs[1];
			OpenScene(sceneFilePath);
		}

		m_EditorCamera = EditorCamera(30.f, 1, .1f, 1000.f);

		m_Delta = 1.f;

		m_Delta = 1 / m_Delta;

		m_SceneHierarchyPanel.SetContext(m_Scene);

		m_MainEditorLayer = this;
	}
	void EditorLayer::OnDetach()
	{
		HZ_PROFILE_FUNCTION();

	}
	float timeStep;
	void EditorLayer::OnUpdate(Timestep& ts)

	{
		HZ_PROFILE_FUNCTION();

		{
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

			m_EditorCamera.OnUpdate(ts);


		}
		Renderer2D::ResetStats();

		m_FrameBuffer->Bind();

		RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
		RenderCommand::Clear();

		m_FrameBuffer->Bind();

		//render to screen
		{
			if (m_Scene->ScenePlay)
			{
				if (!m_Scene->OnUpdateRuntime(ts))
				{
					HZ_ERROR("Tried Rendering without a primary scene camera");
					m_Scene->ScenePlay = false;
				}
			}
			if (!m_Scene->ScenePlay)
			{
				if (m_UsingEditorCamera)
					m_Scene->OnUpdateEditor(ts, m_EditorCamera);
				else
				{
					if (!m_Scene->OnUpdateRuntime(ts))
					{
						m_Scene->OnUpdateEditor(ts, m_EditorCamera);
						m_UsingEditorCamera = true;
						HZ_ERROR("Tried Rendering without a primary scene camera");
					}
				}
			}
		}

		//mouse picking
		{
			auto [mx, my] = ImGui::GetMousePos();
			mx -= m_ViewportBounds[0].x;
			my -= m_ViewportBounds[0].y;// -24;
			auto viewportWidth = m_ViewportBounds[1].x - m_ViewportBounds[0].x;
			auto viewportHeight = m_ViewportBounds[1].y - m_ViewportBounds[0].y;
			my = viewportHeight - my;
			int mousex = (int)mx;
			int mousey = (int)my;
			if (mousex >= 0 && mx < viewportWidth && mousey >= 0 && my < viewportHeight/* - 24*/)
			{
				int pixel = (int)m_FrameBuffer->Pixel(1, mousex, mousey);
				//HZ_CORE_ERROR("{0}", pixel);

				if (pixel == -1)
					m_HoveredEntity = Entity();
				else
					m_HoveredEntity = { pixel, m_Scene.get() };

			}
		}


		m_FrameBuffer->UnBind();
		timeStep = ts.GetSeconds();
	}



	void EditorLayer::OnImGuiRender()
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
					// Disabling full screen would allow the window to be moved to the front of other windows,
					// which we can't undo at the moment without finer window depth/z control.
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

					if (ImGui::MenuItem("reset Scene"))
					{
						m_Scene = NewRef<Scene>();
						m_Scene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
						m_SceneHierarchyPanel.SetContext(m_Scene);
					}

					if (ImGui::MenuItem("Open...", "Ctrl+O"))
						OpenScene();


					if (ImGui::MenuItem("Deserialize"))
					{
						SceneSerializer serializer(m_Scene);
						serializer.Deserialize("assets/scenes/Example.hazel");
					}

					if (ImGui::MenuItem("Serialize"))
					{
						SceneSerializer serializer(m_Scene);
						serializer.Serialize("assets/scenes/Example.hazel");
					}

					if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
						SaveSceneAs();

					if (ImGui::MenuItem("Exit"))
						Application::Get().Close();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

		}

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		//settings
		{
			ImGui::Begin("Settings");
			//ImGui::SliderFloat("angle", &m_angle, 0, 2 * 3.1416f);
			ImGui::Checkbox("using editor camera", &m_UsingEditorCamera);
			int size = (int)fbspec.Attatchments.Attachments.size();
			for (int i = size - 1; i >= 0; i--)
			{
				if (!(fbspec.Attatchments.Attachments[i].m_TextureFormat == FramebufferTextureFormat::RGBA8))
					size--;
			}
			if (size > 1)
				ImGui::SliderInt("colorBuffer", &colorBufferIndex, 0, size - 1);

			auto& stats = Renderer2D::GetStats();

			ImGui::TextColored({ .8f, .2f, .2f, 1.f }, "Draw Calls: %d", Renderer2D::GetStats().drawCalls);
			ImGui::TextColored({ .8f, .2f, .2f, 1.f }, "Quad Count: %d", Renderer2D::GetStats().quadCount);
			ImGui::TextColored({ .8f, .2f, .2f, 1.f }, "frame buffer width: %d", m_FrameBuffer->GetSpecs().Width);
			ImGui::TextColored({ .8f, .2f, .2f, 1.f }, "framebuffer height: %d", m_FrameBuffer->GetSpecs().Height);


			std::string name = "Null";
			if (m_HoveredEntity && m_HoveredEntity.HasComponent<Component::Tag>())
				name = m_HoveredEntity.GetComponent<Component::Tag>().name;
			ImGui::Text("hovered entity: %s", name.c_str());

			std::string mode;
			switch (m_GizmoType)
			{
			case -1:
				mode = "none";
				break;
			case 0:
				mode = "Translation";
				break;
			case 1:
				mode = "Rotation";
				break;
			case 2:
				mode = "Scale";
				break;
			}
			ImGui::Text("ImGuizmo mode: %s", mode.c_str());

			if (Input::IsKeyPressed(KeyCode::LeftAlt) && m_UsingEditorCamera)
				ImGui::Text("Mouse mode: Camera Edit\n\t(lft mouse rotate, right zoom, middle pan)");
			else
				ImGui::Text("Mouse mode: Scene Edit");

			static float prevTs = timeStep;

			static bool vsync = true;
			if (ImGui::Checkbox("vsync?", &vsync))
			{
				Application::Get().GetWindow().SetVSync(vsync);
				prevTs = 0.1f;
			}
			static short frames = 0;
			{
				float times = (1 / 6.0f) / (prevTs + 0.00001f);
				frames = (int)fmod(frames, times);
				if (++(frames) == 1)
					prevTs = timeStep;
			}
			ImGui::TextColored({ .8f, .2f, .8f, 1.f }, "frameRate: %f-fps\n\t%f-seconds/frame", 1 / prevTs, prevTs);

			ImGui::Text("camera yaw:%f, pitch:%f, dist:%f", m_EditorCamera.GetYaw(), m_EditorCamera.GetPitch(), m_EditorCamera.GetDistance());
			if (ImGui::Button("reset editor camera to defaults?"))
				m_EditorCamera = EditorCamera(m_EditorCamera);


			if (Input::IsMouseButtonPressed(Mouse::ButtonLeft) && !Input::IsKeyPressed(KeyCode::LeftAlt)
				&& !ImGuizmo::IsOver() && m_SceneHovered)
			{
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
				m_SceneHovered = false;
			}
			ImGui::End();
		}


		//viewport, main scene
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			ImGui::Begin("viewPort");


			auto viewportOffset = ImGui::GetCursorPos(); //includes tab bar
			m_ViewPortFocused = ImGui::IsWindowFocused();
			m_ViewPortHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused && !m_ViewPortHovered);

			auto viewPortSize = ImGui::GetContentRegionAvail();
			m_ViewPortSize = { viewPortSize.x, viewPortSize.y };

			//drawing the scene to imgui
			ImGui::Image((void*)(uint64_t)m_FrameBuffer->GetColorAttachmentID(colorBufferIndex),
				{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });


			m_SceneHovered = ImGui::IsItemHovered();


			auto windowSize = ImGui::GetWindowSize();
			ImVec2 minBound = ImGui::GetWindowPos();
			minBound.x += viewportOffset.x;
			minBound.y += viewportOffset.y;

			ImVec2 maxBound = { minBound.x + windowSize.x - viewportOffset.x, minBound.y + windowSize.y - viewportOffset.y };
			m_ViewportBounds[0] = { minBound.x, minBound.y };
			m_ViewportBounds[1] = { maxBound.x, maxBound.y };

			Input::SetViewportOffset(minBound.x, minBound.y);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					OpenScene(std::filesystem::path(g_AssetPath) / path);
				}
				ImGui::EndDragDropTarget();
			}

			//Gizmo's
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			if (selectedEntity && m_GizmoType != -1)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				// Camera

				const glm::mat4* cameraProjection;
				glm::mat4 cameraView;
				if (!m_UsingEditorCamera)
				{
					auto cameraEntity = m_Scene->GetPrimaryCameraEntity();
					const auto& camera = cameraEntity.GetComponent<Component::Cameras>().camera;
					cameraProjection = &camera.GetProjection();
					cameraView = glm::inverse(cameraEntity.GetComponent<Component::Transform>().GetTransform());
				}
				else
				{
					cameraProjection = &m_EditorCamera.GetProjection();
					cameraView = m_EditorCamera.GetViewMatrix();
				}

				// Entity transform
				auto& tc = selectedEntity.GetComponent<Component::Transform>();
				glm::mat4 transform = tc.GetTransform();

				bool snapping = Input::IsKeyPressed(Key::LeftControl);
				float snapValue = (m_GizmoType == ImGuizmo::OPERATION::ROTATE) ? 45.f : .5f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(*cameraProjection), (ImGuizmo::OPERATION)m_GizmoType,
					ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, (snapping) ? snapValues : nullptr);

				if (ImGuizmo::IsUsing() && !Input::IsKeyPressed(KeyCode::LeftAlt))
					Math::DecomposeTransform(transform, tc);



			}
			ImGui::End();
		}


		ImGui::PopStyleVar();
		ImGui::End();
	}



	void EditorLayer::OnEvent(Event& e)
	{
		m_Camera.OnEvent(e);
		//if (e.GetEventType() == EventType::MouseButtonPressed)
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));

	}
	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);
		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				NewScene();

			break;
		}
		case Key::O:
		{
			if (control)
				OpenScene();

			break;
		}
		case Key::S:
		{
			if (control && shift)
				SaveSceneAs();

			break;
		}
		case Key::Q:
		{
			m_GizmoType = -1;
			break;
		}
		case Key::W:
		{
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case Key::E:
		{
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		case Key::R:
		{
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
		case Key::Delete:
		{
			Entity tmp = { m_SceneHierarchyPanel.GetSelectedEntity(), m_Scene.get() };
			m_Scene->DestroyEntity(tmp);
			m_SceneHierarchyPanel.SetSelectedEntity(Entity());
		}
		}
		return true;
	}

	void EditorLayer::NewScene()
	{
		m_Scene = NewRef<Scene>();
		m_Scene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		m_SceneHierarchyPanel.SetContext(m_Scene);
	}
	void EditorLayer::OpenScene()
	{
		m_SceneHierarchyPanel.SetSelectedEntity({});

		std::string filepath = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");


		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		std::string defaultFilepath = "assets\\scenes\\";
		std::string filepath = path.string();
		if ((filepath.size() <= defaultFilepath.size() || filepath.substr(0, defaultFilepath.size()) != defaultFilepath))
			filepath = defaultFilepath + filepath;

		if (!m_Scene->Empty())
			m_Scene = NewRef<Scene>();
		m_Scene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		m_SceneHierarchyPanel.SetContext(m_Scene);

		SceneSerializer serializer(m_Scene);
		if (!serializer.Deserialize(filepath))
		{
			if (!serializer.Deserialize(filepath.substr(defaultFilepath.size())))
				HZ_CORE_ERROR("invalid file:\n\t{0},\nor at:\t{1}\n please try again.", filepath, filepath.substr(defaultFilepath.size()));
		}
	}
	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_Scene);
			serializer.Serialize(filepath);
		}
	}


}