#pragma once

#include "Hazel.h"
#include "Panels/SceneHierarchyPanel.h"

namespace Hazel
{

	class EditorLayer : public Layer
	{
	public:
		EditorLayer(std::string filePath);
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep & ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event & e) override;
		
		void OpenScene(std::string filepath = "");
	public:
		static EditorLayer* m_MainEditorLayer;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMousePressed(MouseButtonPressedEvent& e);

		void NewScene();
		void SaveSceneAs();
	private:
		OrthographicCameraController m_Camera;

		Ref<VertexArray> m_VertexArray;
		Ref<Shader> m_Shader;
		Ref<Framebuffer> m_FrameBuffer;

		FramebufferSpecs fbspec;
		int colorBufferIndex = 0;

		EditorCamera m_EditorCamera;


		Ref<Scene> m_Scene;
		Entity m_HoveredEntity, m_MouseCursor;

		Ref<Texture2D> m_checkerboard;

		glm::vec4 m_SquareColor = { 1, 0, 1, 1 };

		glm::vec2 m_ViewPortSize = { -1, -1 };
		glm::vec2 m_ViewportBounds[2];

		float m_angle = 0, m_Delta = .5;

		bool m_ViewPortFocused = false, m_ViewPortHovered = false, m_UsingEditorCamera = true;
		int m_GizmoType = 0;

		std::string m_FilePath;

		//panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};

}