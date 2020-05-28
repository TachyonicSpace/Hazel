#pragma once

#include "Hazel/Core/Timestep.h"
#include "Hazel/Renderer/Camera.h"
		  
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"


namespace Hazel {

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false); // 2 units

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() { return m_ZoomLevel; }
		const float GetZoomLevel() const { return m_ZoomLevel; }

		void SetZoomLevel(float zoom) { m_ZoomLevel = zoom; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0;
		OrthographicCamera m_Camera;

		bool m_Rotation;
		float m_CameraRotation = 0;
		glm::vec3 m_Pos = glm::vec3(0.f, 0.f, 0.f);
		float m_TranslationSpeed = 1;
		float m_RotationSpeed = 180;
	};

}