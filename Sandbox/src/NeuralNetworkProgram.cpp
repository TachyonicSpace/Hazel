#include <Hazel.h>
#include "TrainingData.cpp"
#include <random>

#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>

class NeuralNetworkLayer : public Hazel::Layer
{
public:
	NeuralNetworkLayer()
		:Layer("2D sandbox"), m_Camera(Hazel::Application::Get().GetWindow().GetAspectRatio())
	{
		nn = network(layers);
		data.reserve(16);
		data.push_back({ {0, 0}, {0, 0} });
		data.push_back({ {0, 1}, {0, .5} });
		data.push_back({ {1, 0}, {0, 1} });
		data.push_back({ {1, 1}, {0, 0} });
		data.push_back({ {.25, .25}, {1, .5} });
		data.push_back({ {.25, .75}, {1, 1} });
		data.push_back({ {.75, .25}, {1, 0} });
		data.push_back({ {.75, .75}, {1, .5} });
		data.push_back({ {.5, .5}, {0, 1} });

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


		Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());

		float delta0 = 1 / (float)(delta - 1);
		for (float i = 0; i <= 1.00001; i += delta0)
		{
			for (float j = 0; j <= 1.00001; j += delta0)
			{
				glm::vec2 pos = { i, j };
				auto output = nn.input({ i, j });
				Hazel::Color result(0);

				if (toggle == 0)
				{
					result += Hazel::Color(output.x, 0, 0);
				}
				else if (toggle == 1)
				{
					result += Hazel::Color(0, 0, output.y);
				}
				else
					result += Hazel::Color(output.x, 0, output.y);


				for (auto& targets : data)
				{
					auto posi = targets.input;
					if (posi.x > pos.x - delta0 / 2.f && posi.x <= pos.x + delta0 / 2.f && posi.y > pos.y - delta0 / 2.f && posi.y <= pos.y + delta0 / 2.f)
					{
						if (showTargets)
						{
							if (toggle == 0) { result = Hazel::Color(targets.output.x, 0, 0); }
							else if (toggle == 1)
							{
								result = Hazel::Color(0, 0, targets.output.y);
							}
							else
								result = Hazel::Color(targets.output.x, 0, targets.output.y);


							break;
						}
						result += Hazel::Color(0, 1, 0);
						break;
					}
				}

				Hazel::Renderer2D::DrawQuad(glm::vec2(map(i, 0, 1, -(1 - delta0), (1 - delta0)), map(j, 0, 1, -(1 - delta0), (1 - delta0)))*.99f, 2.f * glm::vec2(delta0, delta0), result);
				result = Hazel::Color(0);
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


			auto stats = Hazel::Renderer2D::GetStats();
			ImGui::Text("draw calls: %d", stats.drawCalls);
			ImGui::Text("\nFrame Rate: %f, frameTime: %f", 1 / timeStep, timeStep);
			int index = 0;




			if (data.size() != 0)
			{
				const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
					ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
				ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImGui::Separator();
				bool open = ImGui::TreeNodeEx((void*)&data, treeNodeFlags, "Training Data");
				ImGui::PopStyleVar();

				if (open)
				{
					for (int i = 0; i < data.size(); i++)
					{
						auto& points = data[i];
						ImGui::SliderFloat4(("point#" + std::to_string(index++)).c_str(), &points.input.x, 0, 1);
						ImGui::Text("\tError: %f, %f", points.error(nn).x, points.error(nn).y);
						ImGui::SameLine();
						if (ImGui::Button(std::string("remove Data Point##" + i).c_str()))
						{
							data.erase(data.begin() + i);
						}
					}
					ImGui::TreePop();
				}


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
			ImGui::Checkbox("show what target values are suppose to be?", &showTargets);


			static std::string funcString[] = { "Sigmoid", "Tanh", "LeakyReLU", "Elu", "Swish", "SoftMax", "Linear"};
			auto selectedfunc = funcString[currentFunc];
			if (ImGui::BeginCombo("Activation Function", selectedfunc.c_str()))
			{
				for (int i = 0; i < 7; i++)
				{
					bool isSelected = selectedfunc == funcString[i];
					if (ImGui::Selectable(funcString[i].c_str(), isSelected))
					{
						currentFunc = (Activationfunctions)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			ImGui::DragFloat("learningRate", &nn.learningRate, .001, .001, 1.001);
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
	int layers = 1, delta = 50;

	float timeStep = 0.f;
	int toggle = 0;
	bool train = true;
	bool showTargets = false;

	Hazel::Ref<Hazel::Framebuffer> m_FrameBuffer;
	std::vector<TrainingData> data;
};
