#include "hzpch.h"
#include "Application.h"

#include "input.h"

#include "GLFW/glfw3.h"

namespace Hazel {

	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Hazel::Application::Application(const WindowProps& props)
	{
		HZ_PROFILE_FUNCTION();

		HZ_CORE_ASSERT(!s_Instance, "Application already exist!!");
		s_Instance = this;

		m_Window = Window::Create(props);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.handled)
				break;
		}
	}

	Hazel::Application::~Application()
	{
		HZ_PROFILE_FUNCTION();

	}

	void Hazel::Application::Run()
	{
		HZ_PROFILE_FUNCTION();

		while (m_Running)
		{
			HZ_PROFILE_SCOPE("run Loop");

			float time = (float)glfwGetTime();
			Timestep ts = time - m_LastFrameTime;
			m_LastFrameTime = time;


			if (!m_Minimized)
			{
				
				{
					HZ_PROFILE_SCOPE("layerStack update");

					for (Layer* l : m_LayerStack)
						l->OnUpdate(ts);
				}



				m_ImGuiLayer->Begin();
				{
					HZ_PROFILE_SCOPE("layerStack imgui update");

					for (Layer* l : m_LayerStack)
						l->OnImGuiRender();
				}
				m_ImGuiLayer->End();


			}
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		HZ_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

}