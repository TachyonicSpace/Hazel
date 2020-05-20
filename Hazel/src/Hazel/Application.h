#pragma once

#include "Core.h"
#include "Core/Timestep.h"

#include "Window.h"
#include "Hazel\LayerStack.h"
#include "Events\Event.h"
#include "Hazel\Events\ApplicationEvent.h"

#include "Renderer/Camera.h"

#include "Hazel/ImGui/ImGuiLayer.h"

#include "Hazel/Renderer/Shader.h"
#include "Renderer/Renderer.h"

#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

namespace Hazel {

	class HAZEL_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::shared_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer = nullptr;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0;
	private:
		static Application* s_Instance;
	};
	//to be defined in client
	Application* CreateApplication();
}