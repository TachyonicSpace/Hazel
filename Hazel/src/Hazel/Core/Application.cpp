#include "hzpch.h"
#include "Application.h"

#include "Hazel/Renderer/Renderer.h"

#include "input.h"

#include "GLFW/glfw3.h"

namespace Hazel {

	//allows convenient ways to bind events to custom functions
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	//application pointer
	Application* Application::s_Instance = nullptr;

	Hazel::Application::Application(const WindowProps& props)
	{
		HZ_PROFILE_FUNCTION();

		//prevents multiple applications from ruinning
		HZ_CORE_ASSERT(!s_Instance, "Application already exist!!");
		s_Instance = this;

		//creates a window with events
		m_Window = Window::Create(props);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		//starts the renderer to allow drawing
		Renderer::Init();

		//makes a layer to allow for imgui to be used
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Hazel::Application::~Application()
	{
		HZ_PROFILE_FUNCTION();

		Renderer::Shutdown();
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

		//allows for window events to happen
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		//runs events through all layers, as long as event isnt handled
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Hazel::Application::Run()
	{
		HZ_PROFILE_FUNCTION();
		
		
		while (m_Running)
		{

			HZ_PROFILE_SCOPE("run Loop");

			//gets the time of each frame
			float time = (float)glfwGetTime();
			Timestep ts = time - m_LastFrameTime;
			m_LastFrameTime = time;

			//stops updating frame if window is minimized
			if (!m_Minimized)
			{
				//renders main scene first
				{
					HZ_PROFILE_SCOPE("layerStack update");

					for (Layer* l : m_LayerStack)
						l->OnUpdate(ts);
				}


				//then renders imgui stuff
				m_ImGuiLayer->Begin();
				{
					HZ_PROFILE_SCOPE("layerStack imgui update");

					for (Layer* l : m_LayerStack)
						l->OnImGuiRender();
				}
				m_ImGuiLayer->End();


			}
			//updates window itself
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