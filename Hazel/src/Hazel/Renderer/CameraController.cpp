#include "hzpch.h"
#include "CameraController.h"

#include "Hazel/Core/input.h"
#include "Hazel/Core/KeyCodes.h"

namespace Hazel {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		:m_AspectRatio(aspectRatio), m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
	{

	}
	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		HZ_PROFILE_FUNCTION();


		if (Input::IsKeyPressed(HZ_KEY_W))
			m_Pos.y += m_TranslationSpeed * ts;

		else if (Input::IsKeyPressed(HZ_KEY_S))
			m_Pos.y -= m_TranslationSpeed * ts;

		if (Input::IsKeyPressed(HZ_KEY_A))
			m_Pos.x -= m_TranslationSpeed * ts;

		else if (Input::IsKeyPressed(HZ_KEY_D))
			m_Pos.x += m_TranslationSpeed * ts;


		if (m_Rotation) {
			if (Input::IsKeyPressed(HZ_KEY_Q))
				m_Rotation += m_RotationSpeed * ts;

			if (Input::IsKeyPressed(HZ_KEY_E))
				m_Rotation -= m_RotationSpeed * ts;

			m_Camera.SetRotation(m_Rotation);
		}

		m_Camera.SetPosition(m_Pos);

		m_RotationSpeed = m_ZoomLevel;
	}
	void OrthographicCameraController::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();

		EventDispatcher dispatch(e);
		HZ_DISPATCH_EVENT(OrthographicCameraController, MouseScrolled);
		HZ_DISPATCH_EVENT(OrthographicCameraController, WindowResize);
	}

	void OrthographicCameraController::CalculateView()
	{
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		HZ_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset();
		m_ZoomLevel = __min(__max(m_ZoomLevel, .025f), 25);
		CalculateView();
		return false;
	}
	bool OrthographicCameraController::OnWindowResize(WindowResizeEvent& e)
	{
		HZ_PROFILE_FUNCTION();

		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		CalculateView();
		return false;
	}
}