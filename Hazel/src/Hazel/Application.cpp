#include "hzpch.h"
#include "Application.h"

#include "Hazel\Events\ApplicationEvent.h"
#include "Hazel\log.h"

namespace Hazel {

	Hazel::Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
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