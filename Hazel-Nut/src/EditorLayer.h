#pragma once

#include "Hazel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

namespace Hazel
{

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep & ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event & e) override;
		
		void OpenScene(); 
		void EditorLayer::OpenScene(const std::filesystem::path& path);		
		void SaveScene();
	public:
		static EditorLayer* m_MainEditorLayer;
	private:
		void ImGuiMenuBar();
		void Settings();
		void viewportSettings();

		bool OnKeyPressed(KeyPressedEvent& e);

		void NewScene();
		void SaveSceneAs();


		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();
	private:
		OrthographicCameraController m_Camera;

		Ref<VertexArray> m_VertexArray;
		Ref<Shader> m_Shader;
		Ref<Framebuffer> m_FrameBuffer;

		FramebufferSpecs fbspec;
		int colorBufferIndex = 0;

		EditorCamera m_EditorCamera;


		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		Entity m_HoveredEntity;

		Ref<Texture2D> m_checkerboard;

		glm::vec4 m_SquareColor = { 1, 0, 1, 1 };

		glm::vec2 m_ViewPortSize = { -1, -1 };
		glm::vec2 m_ViewportBounds[2];

		float m_angle = 0, m_Delta = .5;

		bool m_ViewPortFocused = false, m_ViewPortHovered = false, entitySelected = false;
		bool m_SceneHovered = false;
		int m_GizmoType = 0;

		std::string m_FilePath;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconStop, m_IconSimulate;
	};

}