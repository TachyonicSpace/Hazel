#include <Hazel.h>

//#define useBias
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

	AI::Matrix CreateInputMatrix(float x, float y)
	{
		return { {x, y, x * x, y * y, x * x * x, y * y * y} };
	}

	void OnAttach()
	{
		n = new AI::FFNeuralNet();
		int numberOfInputs = CreateInputMatrix(0, 0).shape()[1];
		n->Init(numberOfInputs, 1);
		n->AddLayer(7);
		n->AddLayer(3);
		//n->AddLayer(12);
		//std::cout << n->toString();

		resetTrainingData(true);

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

		if (data != nullptr)
		{
			for (int r = 0; r < data->tData.size(); r++)
			{
				Hazel::Color result = Hazel::Color::Lime;
				if (showTargets)
				{
					result = Hazel::Color(data->output(r, 0));
				}

				Hazel::Renderer2D::DrawQuad(glm::vec3(data->input(r, 0) * 2 - 1, data->input(r, 1) * 2 - 1, .02f) * .99f, 2.f * glm::vec2(delta0, delta0), result);
			}
		}

		if (showOutput)
		{
			for (float i = 0; i <= 1.00001; i += delta0)
			{
				for (float j = 0; j <= 1.00001; j += delta0)
				{
					glm::vec2 pos = { i, j };

					auto output = n->ForwardProp(CreateInputMatrix(i, j));//nn.input({ i, j });
					Hazel::Color result(output(0, 0));



					Hazel::Renderer2D::DrawQuad(glm::vec2(map(i, 0, 1, -(1 - delta0), (1 - delta0)), map(j, 0, 1, -(1 - delta0), (1 - delta0))) * .99f, 2.f * glm::vec2(delta0, delta0), result);
					result = Hazel::Color(0);
				}
			}
		}

		if (train)
		{
			HZ_PROFILE_SCOPE("Training Network");

			for (int i = 0; i < epochs; i++)
			{
				data->train(n);
				//data->feedforward(n);
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




			/*if (data.size() != 0)
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


			}*/





			ImGui::SliderInt("delta", &delta, 2, 250);

			glm::vec2 mousePos = { Hazel::Input::GetMouseX(), 500.f - Hazel::Input::GetMouseY() };
			mousePos /= 500.f;
			ImGui::Text("MouseX: %f, MouseY: %f", mousePos.x, mousePos.y);

			//todo: add ability to remove layers or add layers in specific places


			if (ImGui::Button("reset output"))
				n->randomizeMatricies();
			ImGui::SameLine();
			if (ImGui::Button("reset training Data"))
				resetTrainingData(true);

			ImGui::Checkbox("training", &train);
			ImGui::SameLine();
			ImGui::Checkbox("ShowOutputGrid", &showOutput);

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
			ImGui::DragInt("Epochs", &epochs, 5, 1, 200);

			if(data)
				ImGui::DragFloat("Training Data Noise", &data->tdNoise, .001f, 0.f, 1.f);
			//ImGui::Text("Network Values:\n\n%s", n->toString().c_str());


			{
				static int newLayerSize = 3;
				for (int i = 1; i < n->layers.size() - 1; i++)
				{
					ImGui::Text(n->layers[i]->toString().c_str());
					if (ImGui::Button("Remove layer " + i))
						n->RemoveNthHiddenLayer(i);
				}

				ImGui::Text(n->layers[n->layers.size() - 1]->toString().c_str());
				ImGui::InputInt("New Layer Size", &newLayerSize);
				if (ImGui::Button("Add Layer"))
					n->AddLayer(newLayerSize);
			}
			ImGui::End();
		}

	}

	void resetTrainingData(bool useDefaultData)
	{
		if (!useDefaultData)
		{
			delete data;
			data = nullptr;
			return;
		}
		data = new BatchedTrainingData(AI::TrainingData(CreateInputMatrix(0, 0), AI::Matrix({ {0} })));
		data->addLabeledData(AI::TrainingData(CreateInputMatrix(0, 1), AI::Matrix({ {1} })));
		data->addLabeledData(AI::TrainingData(CreateInputMatrix(1, 0), AI::Matrix({ {1} })));
		data->addLabeledData(AI::TrainingData(CreateInputMatrix(1, 1), AI::Matrix({ {0} })));

		data->addLabeledData(AI::TrainingData(CreateInputMatrix(.25f, .25f), AI::Matrix({ {.95f} })));
		data->addLabeledData(AI::TrainingData(CreateInputMatrix(.25f, .75f), AI::Matrix({ {.05f} })));
		data->addLabeledData(AI::TrainingData(CreateInputMatrix(.75f, .25f), AI::Matrix({ {.05f} })));
		data->addLabeledData(AI::TrainingData(CreateInputMatrix(.75f, .75f), AI::Matrix({ {.95f} })));

		//data.push_back(AI::TrainingData(AI::Matrix({ {.50f, .50f} }), AI::Matrix({ {1} })));
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

			if (Hazel::Input::IsKeyPressed(Hazel::KeyCode::R))
			{
				resetTrainingData(false);
				return;
			}


			AI::Matrix mousePos = CreateInputMatrix(Hazel::Input::GetMouseX() / 500.f, (500.f - Hazel::Input::GetMouseY()) / 500.f);
			if (data != nullptr)
				data->addLabeledData(AI::TrainingData(mousePos, output));
			else
				data = new BatchedTrainingData(AI::TrainingData(mousePos, output));
		}
	}
private:

	Hazel::OrthographicCameraController m_Camera;
	AI::FFNeuralNet* n;

	struct BatchedTrainingData
	{
		AI::Matrix input, output;
		std::vector<AI::TrainingData> tData;
		float tdNoise = 0.02f;

		BatchedTrainingData(AI::TrainingData d)
			:input(1, 1), output(1, 1)
		{
			input = d.input;
			output = d.output;
			this->tData.push_back(d);
		}

		void addLabeledData(AI::TrainingData d)
		{
			input = input.PushMatrixIntoRows(d.input);
			output = output.PushMatrixIntoRows(d.output);
			this->tData.push_back(d);
		}

		size_t size() { return tData.size(); }

		void train(AI::FFNeuralNet* nn, int batchsize = 0)
		{
			if (tData.size() != 0)
			{
				ShuffleTrainingData(batchsize);
				nn->BackProp(input, output, tData.size());
			}
		}

		void ShuffleTrainingData(int batchSize = 0)
		{
			if (batchSize == 0)
				batchSize = tData.size();

			for (int i = 0; i < 10 * batchSize; i++)
			{
				int a = rand() % batchSize;
				int b = rand() % batchSize;

				AI::TrainingData tmp = tData[a];
				tData[a] = tData[b];
				tData[b] = tmp;
			}
			AI::Matrix shuffledInput = tData[0].input;
			AI::Matrix shuffledOutput = tData[0].output;
			for (int i = 1; i < batchSize; i++)
			{
				shuffledInput = shuffledInput.PushMatrixIntoRows(tData[i].input);
				shuffledOutput = shuffledOutput.PushMatrixIntoRows(tData[i].output);
			}
			input = shuffledInput.addNoise(tdNoise);
			output = shuffledOutput;
		}

		void feedforward(AI::FFNeuralNet* nn, int batchsize = 0)
		{
			ShuffleTrainingData(batchsize);
			nn->ForwardProp(input);
		}
	};
	BatchedTrainingData* data;

	int delta = 50, epochs = 100;
	bool showOutput = true;

	float timeStep = 0.f;
	int toggle = 0;
	bool train = true;
	bool showTargets = false;
};