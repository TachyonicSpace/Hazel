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


	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			HZ_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Hazel Application";
		int length, width;
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class  Application {
	public:
		Application(const ApplicationSpecification& appSpecs);
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static Application& Get() { return *s_Instance; }
		ApplicationSpecification GetApplicationSpecification() const { return m_Specification; }
		Window& GetWindow() { return *m_Window; }

		void Close() { m_Running = false; }

		void Restart() {/* m_Restart = true; */}

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		bool m_Minimized = false;
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		ApplicationSpecification m_Specification;
		Ref<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer = nullptr;
		bool m_Running = true;
		bool m_Restart = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0;
	private:
		static Application* s_Instance;

	};
	//to be defined in client
	Application* CreateApplication(ApplicationCommandLineArgs args);
}