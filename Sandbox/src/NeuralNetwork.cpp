#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "glm/gtx/string_cast.hpp"


struct networkLayer
{
	networkLayer(glm::mat4 weightValues)
	{
		weights = weightValues;
		outputs = glm::vec4(0);
	}
	networkLayer() = default;
	networkLayer(glm::mat4 weightValues, glm::vec4 outputValues)
	{
		weights = weightValues;
		outputs = outputValues;
	}

	glm::mat4 weights;
	glm::vec4 outputs;
};

struct network
{
	network::network()
		:hiddenLayersDepth(1), hiddenLayers(0)
	{
		randomizeMatrixies();
	}
	network::network(int depth)
		: hiddenLayersDepth(depth), hiddenLayers(depth)
	{
		randomizeMatrixies();
	}
	void inc()
	{
		hiddenLayersDepth++;
		glm::mat4 tmp = {
			{ randint(), randint(), randint(), randint() },
			{ randint(), randint(), randint(), randint() },
			{ randint(), randint(), randint(), randint() },
			{ randint(), randint(), randint(), randint() }
		};
		hiddenLayers.emplace_back(tmp);
	}

	glm::vec2 input(glm::vec2 inputData)
	{
		if (hiddenLayersDepth == 0)
		{
			return activationFunc(inputToOutput * inputData);
		}

		auto hidden = activationFunc(inputToHidden * inputData);
		hiddenLayers[0].outputs = hidden;

		for (int i = 1; i < hiddenLayers.size(); i++)
		{
			hidden = activationFunc(hiddenLayers[i].weights * hidden);
			hiddenLayers[i].outputs = hidden;
		}
		auto output = hiddenToOutput * hidden;
		return activationFunc(output);
	}

	int hiddenLayersDepth = 0;
	static float activationFunc(float x)
	{
		return 1.f / (1 + std::exp(-x));
	}
	static glm::vec2 activationFunc(glm::vec2 x)
	{
		x.x = activationFunc(x.x);
		x.y = activationFunc(x.y);
		return x;
	}
	static glm::vec4 activationFunc(glm::vec4 x)
	{
		x.x = activationFunc(x.x);
		x.y = activationFunc(x.y);
		x.z = activationFunc(x.z);
		x.w = activationFunc(x.w);
		return x;
	}
	static float iactivationFunc(float x)
	{
		return -std::log(1 / x - 1);
	}
	static glm::vec2 iactivationFunc(glm::vec2 x)
	{
		x.x = iactivationFunc(x.x);
		x.y = iactivationFunc(x.y);
		return x;
	}
	static glm::vec4 iactivationFunc(glm::vec4 x)
	{
		x.x = iactivationFunc(x.x);
		x.y = iactivationFunc(x.y);
		x.z = iactivationFunc(x.z);
		x.w = iactivationFunc(x.w);
		return x;
	}


	void training(glm::vec2 tinput, glm::vec2 expectedOutput)
	{
		auto output = input(tinput);
		auto err = expectedOutput - output;
		if (hiddenLayersDepth == 0)
		{
			auto tmp = err * output;
			tmp *= (glm::vec2(1, 1) - output);
			inputToOutput += glm::mat2x2(tinput.x * tmp, tinput.y * tmp) * learningRate;

			return;
		}

		auto hiddenErrors = glm::transpose(hiddenToOutput) * err;
		long long layerindex = hiddenLayers.size() - 1;
		{
			auto tmp = err * output;
			tmp *= (glm::vec2(1, 1) - output);
			hiddenToOutput += learningRate * glm::transpose(glm::mat2x4(hiddenLayers[layerindex].outputs * tmp.x, hiddenLayers[layerindex].outputs * tmp.y));
		}
		for (; layerindex >= 1; layerindex--)
		{//update
			auto tmp = hiddenErrors * hiddenLayers[layerindex].outputs;
			tmp *= (glm::vec4(1, 1, 1, 1) - hiddenLayers[layerindex].outputs);
			hiddenLayers[layerindex].weights += learningRate * glm::transpose(glm::mat4(hiddenLayers[layerindex].outputs * tmp[0], hiddenLayers[layerindex].outputs * tmp[1],
				hiddenLayers[layerindex].outputs * tmp[2], hiddenLayers[layerindex].outputs * tmp[3]));

			hiddenErrors = glm::transpose(hiddenLayers[layerindex-1].weights) * hiddenErrors;
		}
		{
			auto tmp = hiddenErrors * hiddenLayers[0].outputs;
			tmp *= (glm::vec4(1, 1, 1, 1) - hiddenLayers[0].outputs);
			auto tmp0 = glm::mat2x4(tinput.x * tmp, tinput.y * tmp);
			inputToHidden += learningRate * tmp0;
		}
	}
	void randomizeMatrixies()
	{
		inputToHidden = {	{randint(4.f), randint(4.f), randint(4.f), randint(4.f)},
							{randint(4.f), randint(4.f), randint(4.f), randint(4.f)} };
		inputToOutput = {	{randint(2.f), randint(2.f)},
							{randint(2.f), randint(2.f)} };
		hiddenToOutput = { {randint(2.f), randint(2.f)},
							{randint(2.f), randint(2.f)},
							{randint(2.f), randint(2.f)},
							{randint(2.f), randint(2.f)} };
		for (int i = 0; i < hiddenLayers.size(); i++)
		{
			hiddenLayers[i] = glm::mat4({ {randint(4.f), randint(4.f), randint(4.f), randint(4.f)},
								{randint(4.f), randint(4.f), randint(4.f), randint(4.f)},
								{randint(4.f), randint(4.f), randint(4.f), randint(4.f)},
								{randint(4.f), randint(4.f), randint(4.f), randint(4.f)} });
		}

		//learningRate *= __max(1, hiddenLayersDepth);
	}
	float randint(float max = 1.f)
	{
		return (((float)rand() / (float)RAND_MAX)*2.f-1.f) / std::sqrt(max);
	}

	std::string to_string()
	{
		std::string name = "";

		if (hiddenLayersDepth == 0)
		{
			name += glm::to_string(inputToOutput);
		}
		else
		{
			name += glm::to_string(inputToHidden) + "\n\n\n";
			for (auto& layer : hiddenLayers)
			{
				name += glm::to_string(layer.weights) + "\n\n\n";
			}

			name += glm::to_string(hiddenToOutput);
		}
		/*for(int i = name.find("(("); i != std::string::npos; i = name.find("(("))
		{
			name.replace(i, 2, "\n((");
		}*/
		for (size_t i = name.find("),"); i != std::string::npos; i = name.find("),"))
		{
			name.replace(i, 2, ",\n");
		}	//name.replace(name.begin(), name.end(), "),", "),\n ");
		return name.substr(0, name.find("(")) + "\n" + name.substr(name.find("("));
	}

	glm::mat2x4 inputToHidden;
	glm::mat2 inputToOutput;
	glm::mat4x2 hiddenToOutput;
	std::vector<networkLayer> hiddenLayers;

	float learningRate = .02;
};