#include <Hazel.h>

#include <omp.h>
//#define useBias
#include "NeuralNetwork.h"


#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>


#include <algorithm>
#include <random>

//maps an input range to an output range
static float map(float in, float inMin, float inMax, float outMin, float outMax)
{
	float slope = (outMax - outMin) / (inMax - inMin);
	return outMin + slope * (in - inMin);
}

class FastNNLayer : public Hazel::Layer
{
public:
	FastNNLayer()
		:Layer("2D sandbox"), m_Camera(Hazel::Application::Get().GetWindow().GetAspectRatio())
	{

	}
	~FastNNLayer()
	{

	}
	
	arma::mat CreateInputMatrix(float x, float y)
	{
		return { {x, y, x * x, y * y, cos(x), sin(y)} };
	}
	

	void OnAttach()
	{
		arma::mat C;
		arma::mat D;
		int scale = 1;

		int side1 = 4;
		int side2 = 4;

		C = arma::randu<arma::mat>(side1, side2);
		D = arma::randu<arma::mat>(side2, side1);
		arma::mat A = C * D * scale;
		A.print("Before activation fun");
		A = arma::join_rows(C, D);
		A.print("After activation func");


		n = new AI::FFNeuralNet();
		int numberOfInputs = CreateInputMatrix(0, 0).n_cols;
		n->Init(numberOfInputs, 1);
		n->AddLayer(3 * numberOfInputs);
		n->AddLayer(3 * numberOfInputs);
		//n->AddLayer(12);
		//std::cout << n->toString();

		resetTrainingData(true);
	}
	void OnDetach()
	{
		//delete n;
	}

	void OnUpdate(Hazel::Timestep& ts)
	{
		//arma::mat in({ {1.f, 0.f} });
		//arma::mat target(1, 1);
		//target(0, 0) = .5f;
		//std::cout << in.toString();
		//std::cout << n->ForwardProp(in).toString() + "\n\n";
		//n->BackProp(in, target);
		
		/*
		arma::mat C;
		arma::mat D;
		int scale = 1;

		int side1 = 400;
		int side2 = 400;

		C = arma::randu<arma::mat>(side1, side2);
		D = arma::randu<arma::mat>(side2, side1);
		arma::mat A  = C * D * scale;
		//A.print();
		*/
		
		ellapsedTime += ts.GetSeconds();
		if (true)
		{

			ellapsedTime = 0;
		Hazel::Renderer2D::ResetStats();
			Hazel::RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
			Hazel::RenderCommand::Clear();

			Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());


			float delta0 = 1 / (float)(delta - 1);


			if (showOutput)
			{
				
				if (data != nullptr)
				{
					for (int r = 0; r < data->tData.size(); r++)
					{
						Hazel::Color result = Hazel::Color::Lime;
						if (showTargets)
						{
							result = Hazel::Color((float)data->tData[r].output(0,0));
						}

						Hazel::Renderer2D::DrawQuad(glm::vec3(data->tData[r].input(0, 0) * 2 - 1, data->tData[r].input(0, 1) * 2 - 1, .02f) * .99f, 2.f * glm::vec2(delta0, delta0), result);
					}
				}

				//#pragma omp for collapse(2)//make sure to enable openmp support in project settings
				for (float i = 0; i <= 1.00001; i += delta0)
				{
					for (float j = 0; j <= 1.00001; j += delta0)
					{

						arma::mat output = n->ForwardProp(CreateInputMatrix(i, j));//nn.input({ i, j });
						//Hazel::Color result( (float) A(map(i, 0, 1, 0,  A.n_rows-1), map(j, 0, 1, 0,  A.n_cols-1)) );
						Hazel::Color result((float)output[0, 0]);

						glm::vec2 f = {
							map(i, 0, 1, -(1 - delta0), (1 - delta0)),
							map(j, 0, 1, -(1 - delta0), (1 - delta0))
						};
						Hazel::Renderer2D::DrawQuad(glm::vec2(f.x, f.y) * .99f, 2.f * glm::vec2(delta0, delta0), result);
					}
				}
			}

			Hazel::Renderer2D::EndScene();
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
			



			





			ImGui::SliderInt("delta", &delta, 2, 250);

			glm::vec2 mousePos = { Hazel::Input::GetMouseX(), 500.f - Hazel::Input::GetMouseY() };
			mousePos /= 500.f;
			
			if (ImGui::CollapsingHeader("hide current network state"))
			{
				ImGui::Text("MouseX: %f, MouseY: %f\n\tValue: %f", mousePos.x, mousePos.y, n->ForwardProp(CreateInputMatrix(mousePos.x, mousePos.y))(0,0));
				std::string nodeOutputs = n->toString(true);
				ImGui::TextColored({ 1, 0, .5, 1 }, "Nodes: %s\n", nodeOutputs.c_str());
			}
			//todo: add ability to remove layers or add layers in specific places

			ImGui::Checkbox("Use Default Training Data", &defaultTraining);
			if (ImGui::Button("reset output"))
				n->randomizeMatricies();
			ImGui::SameLine();
			if (ImGui::Button("reset training Data"))
				resetTrainingData(defaultTraining);

			ImGui::Checkbox("training", &train);
			ImGui::SameLine();
			ImGui::Checkbox("ShowOutputGrid", &showOutput);
			if (ImGui::SliderInt("Max Batch Size", &(data->maxBatchSize), 1, data->tData.size()))
				data->ShuffleTrainingData();

			ImGui::Checkbox("show what target values are suppose to be?", &showTargets);
			static int newLayerSize = 3;

			ImGui::InputInt("New Layer Size", &newLayerSize);
			if (ImGui::Button("Add Layer"))
				n->AddLayer(newLayerSize);


			ImGui::DragFloat("learningRate", &n->lr, .001, .001, 1.001);
			ImGui::DragInt("Epochs", &epochs, 5, 1, 200);

			if (data)
				ImGui::DragFloat("Training Data Noise", &data->tdNoise, .001f, 0.f, 1.f);
			if (ImGui::Button("Prune Layers"))
				n->pruneNeurons(.1);

			//manage the layers individually
			for (int l = 1; l < n->layers.size(); l++)
			{
				if (ImGui::CollapsingHeader(("Network Layer: #" + std::to_string(l)).c_str()))
				{
					auto selectedfunc = functionToString[(int)n->layers[l]->function]; 
					if (ImGui::BeginCombo((("Activation Function##index: " + std::to_string(l)).c_str()), selectedfunc.c_str()))
					{
						//ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
						//get the selections shorter
						for (int i = 0; i < 7; i++)
						{
							bool isSelected = selectedfunc == functionToString[i];
							if (ImGui::Selectable(functionToString[i].c_str(), isSelected))
							{
								n->layers[l]->function = (Activationfunctions)i;
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					//ImGui::SameLine();
					if (ImGui::Button(("Remove layer: " + std::to_string(l)).c_str()))
						n->RemoveNthHiddenLayer(l);

					ImGui::Text(n->layers[l]->toString().c_str());
				}
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

		arma::mat white = arma::ones<arma::mat>(1, 1);
		arma::mat black = arma::zeros<arma::mat>(1, 1);
		white.print("white:");
		black.print("Black:");

		data = new BatchedTrainingData(AI::TrainingData(CreateInputMatrix(.5, .5), white));

		//border
		constexpr float wallResolution = 1/20.f;
		for (float i = 0; i < 1; i += wallResolution)
		{
			data->addLabeledData(AI::TrainingData(CreateInputMatrix(0, i), black));
			data->addLabeledData(AI::TrainingData(CreateInputMatrix(1, i), black));
			data->addLabeledData(AI::TrainingData(CreateInputMatrix(i, 0), black));
			data->addLabeledData(AI::TrainingData(CreateInputMatrix(i, 1), black));
		}

		//head
		constexpr float headResolution = 1/30.f;
		constexpr float headRadius = .45;
		for (float a = 0; a < 1; a+=headResolution)
		{
			glm::vec2 point = circlePoints({ .5, .5 }, headRadius, 3.14152f*2*a);
			data->addLabeledData(AI::TrainingData(CreateInputMatrix(point.x, point.y), white));
		}

		//eyes
		constexpr float eyeResolution = 1 / 8.f;
		constexpr float eyeRadius = .09;
			//puples
		data->addLabeledData(AI::TrainingData(CreateInputMatrix(   .366, .688), white));
		data->addLabeledData(AI::TrainingData(CreateInputMatrix(1- .366, .688), white));
		for (float a = 0; a < 1; a += eyeResolution)
		{
			//left eye
			glm::vec2 point = circlePoints({ .366, .688 }, eyeRadius, 3.14152f * 2 * a);
			data->addLabeledData(AI::TrainingData(CreateInputMatrix(point.x, point.y), black));

			//right eye
			point = circlePoints({ 1-.366, .688 }, eyeRadius, 3.14152f * 2 * a);
			data->addLabeledData(AI::TrainingData(CreateInputMatrix(point.x, point.y), black));
		}


		//nose

		//mouth

		//teeth

		data->tData.erase(data->tData.begin());
		data->ShuffleTrainingData();
	}
	
	void OnEvent(Hazel::Event& e)
	{
		if ((e.GetEventType() == Hazel::EventType::KeyPressed || e.GetEventType() == Hazel::EventType::MouseButtonPressed) && Hazel::Input::GetMouseX() <= 500 && Hazel::Input::GetMouseX() >= 0 && Hazel::Input::GetMouseY() <= 500 && Hazel::Input::GetMouseY() >= 0)
		{
			
			
			if (Hazel::Input::IsKeyPressed(Hazel::KeyCode::R))
			{
				resetTrainingData(false);
				return;
			}


			bool white = Hazel::Input::IsMouseButtonPressed(0); //IsKeyPressed(Hazel::Key::W);
			bool black = Hazel::Input::IsMouseButtonPressed(1); //IsKeyPressed(Hazel::Key::S);
			if ( white || black )
			{
				arma::mat output(1, 1);
				if (white)
					output(0, 0) = 1;
				else if (black)
					output(0, 0) = 0;
				arma::mat mousePos = CreateInputMatrix(Hazel::Input::GetMouseX() / 500.f, (500.f - Hazel::Input::GetMouseY()) / 500.f);
				if (data != nullptr)
					data->addLabeledData(AI::TrainingData(mousePos, output));
				else
					data = new BatchedTrainingData(AI::TrainingData(mousePos, output));
			}
			
		}
	}
private:


	
	glm::vec2 circlePoints(glm::vec2 origin, float r, float a)
	{
		return glm::vec2(cos(a)*r, sin(a)*r) + origin;
	}

	Hazel::OrthographicCameraController m_Camera;
	
	AI::FFNeuralNet* n;
	struct BatchedTrainingData
	{
		arma::mat input, output;
		std::vector<AI::TrainingData> tData;
		int maxBatchSize = 20;
		float tdNoise = 0.00f;
		

		BatchedTrainingData(AI::TrainingData d)
			:input(1, 1), output(1, 1)
		{
			input = d.input;
			output = d.output;
			this->tData.push_back(d);
		}

		int getBatchSize()
		{
			return std::min(maxBatchSize, (int)tData.size());
		}

		void addLabeledData(AI::TrainingData d)
		{
			//input = input.PushMatrixIntoRows(d.input);
			//output = output.PushMatrixIntoRows(d.output);
			this->tData.push_back(d);
		}

		size_t size() { return tData.size(); }

		void train(AI::FFNeuralNet* nn)
		{
			if (tData.size() != 0)
			{
				ShuffleTrainingData();
				nn->BackProp(input, output, tData.size());
			}
		}

		void ShuffleTrainingData()
		{
			// first shuffle
			for (int k = 0; k < tData.size(); k++) {
				int r = k + rand() % (tData.size() - k); // careful here!
				std::swap(tData[k], tData[r]);
			}
			
			arma::mat shuffledInput = tData[0].input;
			arma::mat shuffledOutput = tData[0].output;
			//shuffledInput.data.
			for (int i = 1; i < getBatchSize(); i++)
			{
				shuffledInput = arma::join_cols(shuffledInput, tData[i].input);
				shuffledOutput = arma::join_cols(shuffledOutput, tData[i].output);
			}
			input = shuffledInput;// .addNoise(tdNoise);
			output = shuffledOutput;
		}

		void feedforward(AI::FFNeuralNet* nn)
		{
			ShuffleTrainingData();
			nn->ForwardProp(input);
		}
	};
	BatchedTrainingData* data;
	
	int delta = 50, epochs = 100;
	bool showOutput = true;

	float timeStep = 0.f;
	float ellapsedTime = 1E20;
	int toggle = 0;
	bool train = true;
	bool defaultTraining = true;
	bool showTargets = false;
	
};