#include "hzpch.h"
/*#include "NeuralNetwork.h"

namespace Hazel
{
	/*
	std::thread* displaying = nullptr;
	std::thread* training = nullptr;
	bool AppStillRunning = true;

	nc::NdArray<float> X = {
			{0, 0},
			{0, 1},
			{1, 0},
			{1, 1}
	};
	nc::NdArray<float> Y =
		nc::NdArray<float>(
			{ 0,
			  1,
			  1,
			  0 }).transpose();

	nc::NdArray<float> error = nc::ones<float>(Y.shape());

	uint32_t i = X.row(0).size();
	uint32_t h = 10;
	uint32_t o = Y.row(0).size();

	float lr = .01;
	float reg_param = 0;
	//float mat_iterations = 5000;
	float m = Y.size();

	nc::NdArray<float> W1 = nc::random::randN<float>({ h, i });
	nc::NdArray<float> W2 = nc::random::randN<float>({ o, h });

	nc::NdArray<float> b1 = nc::zeros<float>({ h, 1 });
	nc::NdArray<float> b2 = nc::zeros<float>({ o, 1 });


	nc::NdArray<float> sigmoid(nc::NdArray<float> x, bool derv = false)
	{
		if (derv)
			return x * (1.f - x);
		nc::NdArray<float> tmp = x;
		for (auto& n : tmp)
			n = 1 / (1 + nc::powerf(glm::e<double>(), -n));
		return tmp;
	}
	std::tuple<nc::NdArray<float>, nc::NdArray<float>, nc::NdArray<float>> forward(nc::NdArray<float> a1)
	{

		auto z2 = nc::dot(W1, a1) + b1;
		auto a2 = sigmoid(z2);

		auto z3 = nc::dot(W2, a2) + b2;
		auto a3 = sigmoid(z3);


		return { a1, a2, a3 };
	}
	nc::NdArray<float> forwardPredict(nc::NdArray<float> a1)
	{
		if (a1.shape().rows == 1)
		{
			auto& [a, b, c] = forward(a1.transpose());
			//HZ_CORE_WARN("{0}", c[0]);
			return c;
		}
		else
		{
			auto& [a, b, c] = forward(a1);
			//HZ_CORE_WARN("{0}", c[0]);
			return c;
		}
	}


	void train(nc::NdArray<float> inputs, nc::NdArray<float> targets)
	{
		float errorlevel = .02;
		bool stillTraining = true;
		for (int i = 0; i < error.size(); i++)
		{
			stillTraining |= error[i] < errorlevel;
		}

		while (AppStillRunning)
		{
			//train(X.row(i).transpose(), Y.row(i));
			if (stillTraining)
			{
				for (int j = 0; j < 20; j++)
				{
					for (int i = 0; i < m; i++)
					{
						auto a1 = inputs.row(i).transpose();
						auto target = targets.row(i);

						auto& [in, hidden, output] = forward(a1);

						auto dZ2 = output - target;
						auto dW2 = nc::dot(dZ2, hidden.transpose()) / m;
						auto db2 = nc::sum(dZ2, nc::Axis::ROW) / m;

						auto dA1 = nc::dot(W2.transpose(), dZ2);
						auto dZ1 = dA1 * (hidden * (1.f - hidden));
						auto dW1 = nc::dot(dZ1, in.transpose()) / m;
						auto db1 = nc::sum(dZ1, nc::Axis::COL) / m;

						W1 -= (dW1 * lr);
						W2 -= lr * dW2;
						b1 -= lr * db1.transpose();
						b2 -= lr * db2;

						//if (j == 0)
							//showNetwork();
					}
				}
			}
			for (int i = 0; i < error.size(); i++)
			{
				stillTraining |= error[i] < errorlevel;
			}
		}
		HZ_CORE_WARN("training thread ended");
	}
	void display(nc::NdArray<float> inputs, nc::NdArray<float> targets, Ref<Scene> m_Scene)
	{
		#if 0
		while (AppStillRunning)
		{
			m_Scene->each([&](auto entity) {
				auto& scene = *m_Scene;
				if (Entity::HasComponent<Component::Quads>(scene, entity))
				{
					auto& quad = Entity::GetComponent<Component::Quads>(scene, entity).q;
					auto& name = Entity::GetComponent<Component::Tag>(scene, entity).name;
					HZ_CORE_WARN("{0}, {1}", name, name.length());
					float x, y;
					if (name.length() > 4 )
					{
						x = std::stof(name.substr(0, name.find_first_of(",")));
						y = std::stof(name.substr(name.find_first_of(",") + 2));
						auto val = forwardPredict({ x, y })[0];
						quad.colorRef() = (Color)Color::White * val;
						auto tmp = 0;
					}
				}

				});

		}
		HZ_CORE_WARN("updating thread ended");
		#endif
	}

	std::stringstream showNetwork()
	{
		//system("CLS");
		//HZ_CORE_WARN("W1:\n\t {0}, {1}\n\t {2}, {3}\n"
		//	"b1\n\t {4},{5}\n"
		//	"w2:\n\t{6}, {7}\n"
		//	"b2\n\t {8}\n\n\n", W1[0, 0], W1[0, 1], W1[1, 0], W1[1, 1],
		//	b1[0], b1[1], W2[0], W2[1], b2[0]);
		std::stringstream ss;
		for (int i = 0; i < m; i++)
		{
			error[i] = forwardPredict(X.row(i))[0];
		}
		error = Y - error;
		ss << "W1: \n\n" << W1 << "B1: \n\n" << b1 << "W2: \n\n" << W2 << "B2: \n\n" << b2 << "\n\nError: \n\n" << error;
		return ss;
	}

	



		auto space = .0f;
		for (float i = 0; i < 1; i += m_Delta)
		{
			for (float j = 0; j < 1; j += m_Delta)
			{
				std::string name = "";
				name = name + std::to_string(i) + ", " + std::to_string(j);
				auto secondSquare = m_Scene->CreateEntity(name);
				Color c = Color::White;
				Quad q({ i * (1 + space), j * (1 + space) }, { m_Delta, m_Delta }, c);
				secondSquare.AddComponent<Component::Quads>(q);
			}
		}


		training = new std::thread(train, std::ref(X), std::ref(Y));
		displaying = new std::thread(display, std::ref(X), std::ref(Y), std::ref(m_Scene));


		AppStillRunning = false;
		displaying->join();
		training->join();
		//displaying->~thread();
		//training->~thread();






				ImGui::Separator();
		for (int i = 0; i < m; i++)
		{
			std::string label = "targets";
			label += "[" + std::to_string(i) + "]";
			if (ImGui::SliderFloat(label.c_str(), &Y.data()[i], 0, 1))
			{
				AppStillRunning = false;
				displaying->join();
				training->join();

				AppStillRunning = true;
				training = new std::thread(train, std::ref(X), std::ref(Y));
				displaying = new std::thread(display, std::ref(X), std::ref(Y), std::ref(m_Scene));
			}
		}
		std::stringstream network = showNetwork();
		ImGui::Text("%s", network.str().c_str());


}*/