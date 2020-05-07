#pragma once

#include "Core.h"

#include "Window.h"
#include "Hazel\LayerStack.h"
#include "Events\Event.h"
#include "Hazel\Events\ApplicationEvent.h"

namespace Hazel {

	class HAZEL_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
	private:
		std::unique_ptr<Window> m_Window;
		bool OnWindowClose(WindowCloseEvent& e);
		bool m_Running = true;
		LayerStack m_LayerStack;
	};
	//to be defined in client
	Application* CreateApplication();
}