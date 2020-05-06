#include "hzpch.h"
#include "Application.h"

#include "Hazel\Events\ApplicationEvent.h"
#include "Hazel\log.h"

namespace Hazel {

	Hazel::Application::Application()
	{
	}

	Hazel::Application::~Application()
	{
	}

	void Hazel::Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		HZ_CORE_TRACE(e);
		while (true);
	}

}