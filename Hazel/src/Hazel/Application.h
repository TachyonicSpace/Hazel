#pragma once

#include "Core.h"

#include "Window.h"
#include "Hazel\LayerStack.h"
#include "Events\Event.h"
#include "Hazel\Events\ApplicationEvent.h"

#include "Hazel/ImGui/ImGuiLayer.h"

#include "Hazel/Renderer/Shader.h"

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

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer = nullptr;
		bool m_Running = true;
		LayerStack m_LayerStack;


		unsigned int m_VertexBuffer, m_IndexBuffer, m_VertexArray;

		std::unique_ptr<Shader> m_Shader;
	private:
		static Application* s_Instance;
	};
	//to be defined in client
	Application* CreateApplication();
}