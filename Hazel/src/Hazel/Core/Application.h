#pragma once

#include "Window.h"
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

		Window& GetWindow() { return *m_Window; }

		void Close() { m_Running = false; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Ref<Window> m_Window;
		ImGuiStartup* m_ImGuiLayer = nullptr;
		bool m_Running = true;
	};
}