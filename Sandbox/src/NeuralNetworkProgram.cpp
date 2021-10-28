#include <Hazel.h>
#include "NeuralNetwork.cpp"
#include <random>

#include "imgui/imgui.h"

class NeuralNetworkLayer : public Hazel::Layer
{
public:
	NeuralNetworkLayer()
		:Layer("2D sandbox"), m_Camera(Hazel::Application::Get().GetWindow().GetAspectRatio())
	{
	}

	~NeuralNetworkLayer() = default;

	void OnAttach()
	{
		HZ_PROFILE_FUNCTION();

	}

	void OnDetach()
	{
		HZ_PROFILE_FUNCTION();

	}

	void OnUpdate(Hazel::Timestep& ts)
	{
		Hazel::RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
		Hazel::RenderCommand::Clear();


		Hazel::Renderer2D::ResetStats();
		Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());

		for (float i = 0; i <= 1; i+=.01)
		{
			for (float j = 0; j <= 1; j+=.01)
			{
				Hazel::Renderer2D::DrawQuad({ i * 2 - 1, j * 2 - 1 }, { .0098, .0098 }, { 1, 0, 1, 1 });
				Hazel::Renderer2D::DrawQuad({ 0, 0 }, { 1, 1 }, { 1, 0, 1, 1 });
				//HZ_CORE_INFO("{0}\t{1}", i * 4 - 1, j * 2 - 1);
			}
		}
		
		Hazel::Renderer2D::EndScene();
		timeStep = ts.GetSeconds();
	}


	void OnImGuiRender()
	{


		//settings
		{
			ImGui::Begin("Settings");

			ImGui::Text("%f\t%f", Hazel::Input::GetMouseX(), Hazel::Input::GetMouseY());

			ImGui::End();
		}

	}

	void OnEvent(Hazel::Event& e)
	{
		m_Camera.OnEvent(e);
	}


private:
	Hazel::OrthographicCameraController m_Camera;

	float timeStep = 0.f;

	Hazel::Ref<Hazel::Framebuffer> m_FrameBuffer;

};
