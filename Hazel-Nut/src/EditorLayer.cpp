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
		m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");
		m_IconSimulate = Texture2D::Create("Resources/Icons/SimulateButton.png");

		fbspec.Attatchments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbspec.Width = 1280;
		fbspec.Height = 720;
		m_FrameBuffer = Framebuffer::Create(fbspec);

		m_ActiveScene = NewRef<Scene>();

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			std::string sceneFilePath = commandLineArgs[1];
			OpenScene(sceneFilePath);
		}

		m_EditorCamera = EditorCamera(30.f, 1, .1f, 1000.f);

		m_Delta = 1.f;

		m_Delta = 1 / m_Delta;


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


		if (FramebufferSpecs spec = m_FrameBuffer->GetSpecs();
			m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewPortSize.x || spec.Height != m_ViewPortSize.y))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);

			m_Camera.OnResize(m_ViewPortSize.x, m_ViewPortSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		}
		Renderer2D::ResetStats();

		m_FrameBuffer->Bind();

		RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
		RenderCommand::Clear();

		m_FrameBuffer->Bind();

		////render to screen

		switch (m_SceneState)
		{
		case SceneState::Edit:
		{
			if (m_ViewPortHovered)
				m_Camera.OnUpdate(ts);

			m_EditorCamera.OnUpdate(ts);

			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
			break;
		}
		case SceneState::Play:
		{

			m_ActiveScene->UpdateScripts(ts);
			m_ActiveScene->UpdatePhysics(ts);
			if (!m_ActiveScene->OnUpdateRuntime(ts))
			{
				HZ_ERROR("Tried Rendering without a primary scene camera");
				m_ActiveScene->ScenePlay = false;
			}
			break;
		}
		case SceneState::Simulate:
		{
			if (m_ViewPortHovered)
				m_Camera.OnUpdate(ts);

			m_EditorCamera.OnUpdate(ts);

			m_ActiveScene->UpdateScripts(ts);
			m_ActiveScene->UpdatePhysics(ts);
			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
			break;
		}
		}
		/*Renderer2D::BeginScene(EditorCamera(45, (m_ViewPortSize.x / m_ViewPortSize.y), .1, 1000));
		Renderer2D::DrawQuad({ 0, 0 }, { 1, 1 }, 0, { 1, 0, 1 });//todo:add neural network here
		Renderer2D::EndScene();*/
		//{
		//	if (m_ActiveScene->ScenePlay)
		//	{
		//		if (!m_ActiveScene->OnUpdateRuntime(ts))
		//		{
		//			HZ_ERROR("Tried Rendering without a primary scene camera");
		//			m_ActiveScene->ScenePlay = false;
		//		}
		//	}
		//	if (!m_ActiveScene->ScenePlay)
		//	{
		//		if (m_UsingEditorCamera)
		//			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
		//		else
		//		{
		//			if (!m_ActiveScene->OnUpdateRuntime(ts))
		//			{
		//				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
		//				m_UsingEditorCamera = true;
		//				HZ_ERROR("Tried Rendering without a primary scene camera");
		//			}
		//		}
		//	}
		//}

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
					m_HoveredEntity = { pixel, m_ActiveScene.get() };

			}
		}


		m_FrameBuffer->UnBind();
		timeStep = ts.GetSeconds();
	}



	void EditorLayer::OnImGuiRender()
	{
		Application::Get().GetImGuiLayer()->BeginDocking();

		ImGuiMenuBar();

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		Settings();

		//viewport, main scene
		viewportSettings();


		UI_Toolbar();

		Application::Get().GetImGuiLayer()->EndDocking();
	}
	void EditorLayer::ImGuiMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling full screen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("reset Scene"))
				{
					m_ActiveScene = NewRef<Scene>();
					m_ActiveScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
					m_SceneHierarchyPanel.SetContext(m_ActiveScene);
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();


				if (ImGui::MenuItem("Deserialize"))
				{
					SceneSerializer serializer(m_ActiveScene);
					serializer.Deserialize("assets/scenes/Example.hazel");
				}

				if (ImGui::MenuItem("Serialize"))
				{
					SceneSerializer serializer(m_ActiveScene);
					serializer.Serialize("assets/scenes/Example.hazel");
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Change Working Directory"))
					std::filesystem::current_path() = FileDialogs::OpenFolder();

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}
	void EditorLayer::Settings()
	{
		ImGui::Begin("Settings");
		//ImGui::SliderFloat("angle", &m_angle, 0, 2 * 3.1416f);
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

		static std::string modes[] = { "none", "Translation", "Rotation", "Scale" };
		std::string mode = modes[m_GizmoType + 1];
		ImGui::Text("ImGuizmo mode: %s", mode.c_str());
		name = "";
		Entity selected = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selected && selected.HasComponent<Component::Tag>())
			name = "\n\tselected entity: " + selected.GetComponent<Component::Tag>().name;
		ImGui::TextColored({ 1, .5, .98, 1 }, "q:none, w:translation, e:rotation, r:scale%s", name.c_str());

		if (Input::IsKeyPressed(KeyCode::LeftAlt))
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
			&& !ImGuizmo::IsOver() && m_HoveredEntity)
		{
			m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			m_SceneHovered = false;
		}
		ImGui::End();
	}
	void EditorLayer::viewportSettings()
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
				m_SceneHierarchyPanel.SetSelectedEntity({});
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_AssetPath) / path);
			}
			ImGui::EndDragDropTarget();
		}

		//Gizmo's
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (m_SceneState == SceneState::Edit && selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera

			const glm::mat4* cameraProjection;
			glm::mat4 cameraView;
			cameraProjection = &m_EditorCamera.GetProjection();
			cameraView = m_EditorCamera.GetViewMatrix();


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

		ImGui::PopStyleVar();
		ImGui::End();
	}
	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		Ref<Texture2D> icon = m_SceneState != SceneState::Edit ? m_IconStop : m_IconPlay;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f) - ((float)(m_SceneState==SceneState::Edit) * size *.50));
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
				OnSceneStop();
		}
		if (m_SceneState == SceneState::Edit)
		{
			ImGui::SameLine();
			if (ImGui::ImageButton((ImTextureID)m_IconSimulate->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				OnScenePlay();
				m_SceneState = SceneState::Simulate;
			}
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
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
			if (control)
			{
				if (shift)
					SaveSceneAs();
				else
					SaveScene();
			}

			break;
		}

		// Scene Commands
		case Key::D:
		{
			if (control)
				OnDuplicateEntity();

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
			m_SceneHierarchyPanel.GetSelectedEntity().deleted = true;
		}
		}
		return true;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = NewRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);


		m_EditorScenePath = std::filesystem::path();
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
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		if (path.extension().string() != ".hazel")
		{
			HZ_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		std::string defaultFilepath = "assets\\scenes\\";
		std::string filepath = path.string();
		if ((filepath.size() <= defaultFilepath.size() || filepath.substr(0, defaultFilepath.size()) != defaultFilepath))
			filepath = defaultFilepath + filepath;

		m_EditorScene = NewRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		m_SceneHierarchyPanel.SetContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;
		m_EditorScenePath = path;

		SceneSerializer serializer(m_ActiveScene);
		if (!serializer.Deserialize(filepath))
		{
			if (!serializer.Deserialize(filepath.substr(defaultFilepath.size())))
				HZ_CORE_ERROR("invalid file:\n\t{0},\nor at:\t{1}\n please try again.", filepath, filepath.substr(defaultFilepath.size()));
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();

		HZ_CORE_INFO("Saving Scene: {0}", m_EditorScenePath);

	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;


		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;


		m_ActiveScene->OnRuntimeStop();
		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
			m_EditorScene->DuplicateEntity(selectedEntity);
	}
}