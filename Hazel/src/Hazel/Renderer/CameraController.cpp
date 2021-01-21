#include "hzpch.h"
#include "CameraController.h"

#include "Hazel/Core/input.h"
#include "Hazel/Core/KeyCodes.h"

namespace Hazel {

	//makes an ortho camera with aspect ratio and if rotation is enabled
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		:m_AspectRatio(aspectRatio), m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
	{

	}
	//when updated,
	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		HZ_PROFILE_FUNCTION();

		//moves camera left
		if (Input::IsKeyPressed(HZ_KEY_A))
		{
			m_Pos.x += cos((m_CameraRotation)) * m_TranslationSpeed * ts;
			m_Pos.y += sin((m_CameraRotation)) * m_TranslationSpeed * ts;
		}

		//moves camrera right
		else if (Input::IsKeyPressed(HZ_KEY_D))
		{
			m_Pos.x -= cos((m_CameraRotation)) * m_TranslationSpeed * ts;
			m_Pos.y -= sin((m_CameraRotation)) * m_TranslationSpeed * ts;
		}
		//moves camera up
		if (Input::IsKeyPressed(HZ_KEY_W)) 
		{
			m_Pos.x += -sin((m_CameraRotation)) * m_TranslationSpeed * ts;
			m_Pos.y +=  cos((m_CameraRotation)) * m_TranslationSpeed * ts;
		}
		//moves camera down
		else if (Input::IsKeyPressed(HZ_KEY_S))
		{
			m_Pos.x -= -sin((m_CameraRotation)) * m_TranslationSpeed * ts;
			m_Pos.y -= cos((m_CameraRotation)) * m_TranslationSpeed * ts;
		}

		//if rotation is enabled
		if (m_Rotation) {
			//rotates CC
			if (Input::IsKeyPressed(HZ_KEY_Q))
				m_CameraRotation += m_RotationSpeed * ts;

			//rotates clockwise
			if (Input::IsKeyPressed(HZ_KEY_E))
				m_CameraRotation -= m_RotationSpeed * ts;

			//keeps it between 0-2pi
			if (m_CameraRotation > glm::pi<float>())
				m_CameraRotation -= (glm::pi<float>() * 2.0f);
			else if (m_CameraRotation <= -glm::pi<float>())
				m_CameraRotation += (glm::pi<float>() * 2.0f);

			//rotates camera
			m_Camera.SetRotation(m_CameraRotation);
		}

		//moves camera
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
	//resises camera
	void OrthographicCameraController::OnResize(float width, float height)
	{
		HZ_PROFILE_FUNCTION();

		m_AspectRatio = width / height;
		CalculateView();
	}
	//caluculates camera
	void OrthographicCameraController::CalculateView()
	{
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}
	//zooms camera
	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		HZ_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset();
		m_ZoomLevel = __min(__max(m_ZoomLevel, .025f), 25);
		CalculateView();
		return false;
	}
	//resizes camera based on window
	bool OrthographicCameraController::OnWindowResize(WindowResizeEvent& e)
	{
		HZ_PROFILE_FUNCTION();

		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		CalculateView();
		return false;
	}
}