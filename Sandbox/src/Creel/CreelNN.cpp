#include <Hazel.h>
#include "NeuralNetwork.h"


#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>

class CreelLayer : public Hazel::Layer
{
public:
	CreelLayer()
		:Layer("2D sandbox"), m_Camera(Hazel::Application::Get().GetWindow().GetAspectRatio())
	{

	}
	~CreelLayer()
	{

	}

	void OnAttach()
	{
		n = new AI::FFNeuralNet();

		n->Init(2, 1);
		n->AddLayer(4);
		n->AddLayer(4);
		//n->AddLayer(12);
		//std::cout << n->toString();

		data.push_back(AI::TrainingData(AI::Matrix({ {0, 0} }), AI::Matrix({ {0} })));
		data.push_back(AI::TrainingData(AI::Matrix({ {0, 1} }), AI::Matrix({ {1} })));
		data.push_back(AI::TrainingData(AI::Matrix({ {1, 0} }), AI::Matrix({ {1} })));
		data.push_back(AI::TrainingData(AI::Matrix({ {1, 1} }), AI::Matrix({ {0} })));

		data.push_back(AI::TrainingData(AI::Matrix({ {.25f, .25f} }), AI::Matrix({ {.25f} })));
		data.push_back(AI::TrainingData(AI::Matrix({ {.25f, .75f} }), AI::Matrix({ {.75f} })));
		data.push_back(AI::TrainingData(AI::Matrix({ {.75f, .25f} }), AI::Matrix({ {.75f} })));
		data.push_back(AI::TrainingData(AI::Matrix({ {.75f, .75f} }), AI::Matrix({ {.25f} })));

		//data.push_back(AI::TrainingData(AI::Matrix({ {.50f, .50f} }), AI::Matrix({ {1} })));

	}
	void OnDetach()
	{
		delete n;
	}

	void OnUpdate(Hazel::Timestep& ts)
	{
		//AI::Matrix in({ {1.f, 0.f} });
		//AI::Matrix target(1, 1);
		//target(0, 0) = .5f;
		//std::cout << in.toString();
		//std::cout << n->ForwardProp(in).toString() + "\n\n";
		//n->BackProp(in, target);

		Hazel::RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
		Hazel::RenderCommand::Clear();


		Hazel::Renderer2D::ResetStats();


		Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());


		float delta0 = 1 / (float)(delta - 1);


		for (AI::TrainingData td : data)
		{
			Hazel::Color result = Hazel::Color::Lime;
			if (showTargets)
			{
				result = Hazel::Color(td.output(0, 0));
			}
			
			Hazel::Renderer2D::DrawQuad(glm::vec3(td.input(0, 0) * 2 - 1, td.input(0, 1) * 2 - 1, .02f) * .99f, 2.f * glm::vec2(delta0, delta0), result);
		}

		for (float i = 0; i <= 1.00001; i += delta0)
		{
			for (float j = 0; j <= 1.00001; j += delta0)
			{
				glm::vec2 pos = { i, j };

				auto output = n->ForwardProp({ { i, j } });//nn.input({ i, j });
				Hazel::Color result(output(0, 0));



				Hazel::Renderer2D::DrawQuad(glm::vec2(map(i, 0, 1, -(1 - delta0), (1 - delta0)), map(j, 0, 1, -(1 - delta0), (1 - delta0))) * .99f, 2.f * glm::vec2(delta0, delta0), result);
				result = Hazel::Color(0);
			}
		}

		//Hazel::Application::Get().Close();


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
						ImGui::SliderFloat4(("point#" + std::to_string(index++)).c_str(), &points.input(0, 0), 0, 1);
						ImGui::Text("\tError: %f", points.error(n)(0, 0));
						ImGui::SameLine();
						if (ImGui::Button(std::string("remove Data Point##" + i).c_str()))
						{
							data.erase(data.begin() + i);
						}
					}
					ImGui::TreePop();
				}


			}





			ImGui::SliderInt("delta", &delta, 2, 250);

			static float in[2] = { .5,.5 };
			auto output = n->ForwardProp(AI::Matrix({ { in[0], in[1] } }));
			glm::vec2 mousePos = { Hazel::Input::GetMouseX(), 500.f - Hazel::Input::GetMouseY() };
			mousePos /= 500.f;
			ImGui::Text("MouseX: %f, MouseY: %f", mousePos.x, mousePos.y);

			//todo: add ability to remove layers or add layers in specific places

			if (train)
			{
				HZ_PROFILE_SCOPE("Training Network");
				for (int i = 0; i < epochs; i++)
				{
					int* dataIndex = new int[data.size()];
					for (int i = 0; i < data.size(); i++)
					{
						dataIndex[i] = i;
					}
					for (int i = 0; i < data.size(); i++)
					{
						for (int j = 0; j < data.size(); j++)
						{
							unsigned int i0 = rand() % data.size();
							unsigned int j0 = rand() % data.size();
							auto tmp = dataIndex[i0];
							dataIndex[i0] = dataIndex[j0];
							dataIndex[j0] = tmp;
						}
					}
					for (int i = 0; i < data.size(); i++)
					{
						if (data[dataIndex[i]].needsTraining(n))
							data[dataIndex[i]].train(n);
					}
					delete[] dataIndex;
				}
			}
			if (ImGui::Button("reset"))
				n->randomizeMatricies();

			ImGui::SameLine();
			ImGui::Checkbox("training", &train);
			ImGui::Checkbox("show what target values are suppose to be?", &showTargets);

			//todo: add the ability to change layer activation functions
			for (int l = 1; l < n->layers.size(); l++)
			{
				static std::string funcString[] = { "Sigmoid", "Tanh", "LeakyReLU", "Elu", "Swish", "SoftMax", "Linear" };
				auto selectedfunc = funcString[(int)n->layers[l]->function];
				if (ImGui::BeginCombo(("Activation Function##" + l), selectedfunc.c_str()))
				{
					for (int i = 0; i < 7; i++)
					{
						bool isSelected = selectedfunc == funcString[i];
						if (ImGui::Selectable(funcString[i].c_str(), isSelected))
						{
							n->layers[l]->function = (Activationfunctions)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

			}
			ImGui::DragFloat("learningRate", &n->lr, .001, .001, 1.001);
			ImGui::Text("Network Values:\n\n%s", n->toString().c_str());

			ImGui::End();
		}

	}

	void OnEvent(Hazel::Event& e)
	{
		if (e.GetEventType() == Hazel::EventType::KeyPressed && Hazel::Input::GetMouseX() <= 500 && Hazel::Input::GetMouseX() >= 0 && Hazel::Input::GetMouseY() <= 500 && Hazel::Input::GetMouseY() >= 0)
		{
			AI::Matrix output(1, 1);
			if (Hazel::Input::IsKeyPressed(Hazel::Key::W))
				output(0, 0) = 1;
			else if (Hazel::Input::IsKeyPressed(Hazel::Key::S))
				output(0, 0) = 0;


			AI::Matrix mousePos = { { Hazel::Input::GetMouseX() / 500.f, (500.f - Hazel::Input::GetMouseY()) / 500.f } };
			data.push_back(AI::TrainingData(mousePos, output));
		}
	}
private:

	Hazel::OrthographicCameraController m_Camera;
	AI::FFNeuralNet* n;

	std::vector<AI::TrainingData> data;

	int delta = 50, epochs = 100;;

	float timeStep = 0.f;
	int toggle = 0;
	bool train = true;
	bool showTargets = false;
};