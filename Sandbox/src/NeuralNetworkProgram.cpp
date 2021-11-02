#include <Hazel.h>
#include "TrainingData.cpp"
#include <random>

#include "imgui/imgui.h"

class NeuralNetworkLayer : public Hazel::Layer
{
public:
	NeuralNetworkLayer()
		:Layer("2D sandbox"), m_Camera(Hazel::Application::Get().GetWindow().GetAspectRatio())
	{
		nn = network(layers);
		data.reserve(16);
		data.push_back({ {0, 0}, {0, 0} });
		data.push_back({ {0, 1}, {1, 1} });
		data.push_back({ {1, 0}, {1, 1} });
		data.push_back({ {1, 1}, {0, 1} });

	}

	~NeuralNetworkLayer() = default;

	void OnAttach()
	{
		HZ_PROFILE_FUNCTION();

	}

	void OnDetach()
	{
		HZ_PROFILE_FUNCTION();
		data.~vector();
	}

	void OnUpdate(Hazel::Timestep& ts)
	{
		Hazel::RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
		Hazel::RenderCommand::Clear();


		Hazel::Renderer2D::ResetStats();
		//Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());
		//
		//float delta = .01;
		//for (float i = 0; i <= 1; i+=delta)
		//{
		//	for (float j = 0; j <= 1; j+=delta)
		//	{
		//		Hazel::Renderer2D::DrawQuad({ i * 2 - 1, j * 2 - 1 }, { delta*.98, delta*.98 }, { 1, 0, 1, 1 });
		//		Hazel::Renderer2D::DrawQuad({ 1, 0 }, { 1, 1 }, { 1, 0, 1, 1 });
		//		if(j == 1 && i == 1)
		//			HZ_CORE_INFO("{0}\t{1}", i * 4 - 1, j * 2 - 1);
		//	}
		//}
		//
		//Hazel::Renderer2D::EndScene();
		timeStep = ts.GetSeconds();
	}


	void OnImGuiRender()
	{


		//settings
		{
			ImGui::Begin("Settings");


			Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());

			static int delta = 50;
			float delta0 = 1 / (float)delta;
			for (float i = 0; i <= 1; i += delta0)
			{
				for (float j = 0; j <= 1; j += delta0)
				{
					auto output = nn.input({ i, j });
					Hazel::Color result;
					if (toggle == 0)
					{
						result = Hazel::Color(output.x, 0, 0);
					}
					else if (toggle == 1)
					{
						result = Hazel::Color(0, 0, output.y);
					}
					else
						result = Hazel::Color(output.x, 0, output.y, 1);
					Hazel::Renderer2D::DrawQuad({ i * 2 - 1 + delta0 / 2.f, j * 2 - 1 + delta0 / 2.f }, { delta0 * 2, delta0 * 2 }, result);
				}
			}

			Hazel::Renderer2D::EndScene();

			auto stats = Hazel::Renderer2D::GetStats();
			ImGui::Text("draw calls: %d", stats.drawCalls);
			ImGui::Text("\nFrame Rate: %f, frameTime: %f", 1 / timeStep, timeStep);
			int index = 0;
			for (auto& points : data)
			{
				ImGui::SliderFloat4(("point#" + std::to_string(index++)).c_str(), &points.input.x, 0, 1);
				ImGui::Text("\tError: %f, %f", points.error(nn).x, points.error(nn).y);
			}
			static glm::vec2 newPoints[2] = { {.5, .5}, {.5, .5} };
			ImGui::DragFloat4("new point", &newPoints[0].x, .01f, 0, 1);
			ImGui::SameLine();
			if (ImGui::Button("add new point"))
			{
				data.push_back({ newPoints[0], newPoints[1] });
			}

			ImGui::Separator();
			ImGui::SliderInt("delta", &delta, 2, 250);

			static float in[2] = { .5,.5 };
			auto output = nn.input({ in[0], in[1] });
			ImGui::Text("input: %f, %f\n\toutput: %f, %f", in[0], in[1], output.x, output.y);
			ImGui::DragFloat2("inputValues: ", in, 1 / (float)delta, 0, 1);

			if (ImGui::ColorButton("merge output colors", { output.x, 0, output.y, 1 }))
				if (toggle != 2)
					toggle = 2;
				else
					toggle = 0;

			if (toggle == 0)
			{
				ImGui::SameLine();
				if (ImGui::ColorButton("swap output color(xor)", { output.x, 0, 0, 1 }))
					toggle = 1;
			}
			else if (toggle == 1)
			{
				ImGui::SameLine();
				if (ImGui::ColorButton("swap output color(or)", { 0, 0, output.y, 1 }))
					toggle = 0;
			}

			if (train)
			{
				HZ_PROFILE_SCOPE("Training Network");
				for (int i = 0; i < 100; i++)
				{
					for (auto point : data)
					{
						if (point.needsTraining(nn))
							point.train(nn);
					}
				}
			}
			if (ImGui::SliderInt("layers", &layers, 0, 10))
			{
				nn = network(layers);
			}
			ImGui::SameLine();
			if (ImGui::Button("add layer") && layers < 10)
			{
				nn.inc();
				layers++;
			}
			if (ImGui::Button("reset"))
				nn.randomizeMatrixies();

			ImGui::SameLine();
			ImGui::Checkbox("training", &train);

				ImGui::Text("Network Values:\n\n%s", nn.to_string().c_str());

			ImGui::End();
		}

	}

	void OnEvent(Hazel::Event& e)
	{
		m_Camera.OnEvent(e);
	}


private:
	Hazel::OrthographicCameraController m_Camera;
	network nn;
	int layers = 1;

	float timeStep = 0.f;
	int toggle = 0;
	bool train = true;

	Hazel::Ref<Hazel::Framebuffer> m_FrameBuffer;
	std::vector<TrainingData> data;
};
