#pragma once

#include "Hazel.h"

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

	private:
		OrthographicCameraController m_Camera;

		Ref<VertexArray> m_VertexArray;
		Ref<Shader> m_Shader;
		Ref<Framebuffer> m_FrameBuffer;


		Ref<Scene> m_Scene;
		entt::entity m_SquareEntity;


		Ref<Texture2D> m_checkerboard;

		glm::vec4 m_SquareColor = { 1, 0, 1, 1 };

		glm::vec2 m_ViewPortSize = { -1, -1 };

		float m_angle = 0, m_Delta = .5;

		bool m_ViewPortFocused = false;
		bool m_ViewPortHovered = false;

	};

}