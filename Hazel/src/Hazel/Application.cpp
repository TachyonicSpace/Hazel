#include "hzpch.h"
#include "Application.h"

#include "Hazel\Events\ApplicationEvent.h"
#include "Hazel\log.h"

namespace Hazel {


	#define BIND_EVENT_FN(x) std::bind(&Application::OnEvent, this, std::placeholders::_1)

	Hazel::Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	void Application::OnEvent(Event& e)
	{
		HZ_CORE_INFO("{0}", e);
	}

	Hazel::Application::~Application()
	{
	}

	void Hazel::Application::Run()
	{
		while (m_running)
		{
			m_Window->OnUpdate();
		}
	}

}