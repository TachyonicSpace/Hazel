#pragma once

#include "Core.h"
#include "Timestep.h"

#include "Window.h"
#include "LayerStack.h"
#include "Hazel\Events\Event.h"
#include "Hazel\Events\ApplicationEvent.h"

#include "Hazel/Renderer/Camera.h"

#include "Hazel/ImGui/ImGuiLayer.h"

#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Renderer.h"

#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {

	class  Application {
	public:
		Application(const WindowProps& props = {});
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static Application& Get() { return *s_Instance; }
		Window& GetWindow() { return *m_Window; }

		void Close() { m_Running = false; }

		void Restart() {/* m_Restart = true; */}

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Ref<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer = nullptr;
		bool m_Running = true;
		bool m_Minimized = false;
		bool m_Restart = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0;
	private:
		static Application* s_Instance;
	};
	//to be defined in client
	Application* CreateApplication(std::string str);
}