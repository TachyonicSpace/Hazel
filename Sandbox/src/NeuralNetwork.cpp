#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "glm/gtx/string_cast.hpp"

//maps an input range to an output range
static float map(float in, float inMin, float inMax, float outMin, float outMax)
{
	float slope = (outMax - outMin) / (inMax - inMin);
	return outMin + slope * (in - inMin);
}

//hold the info of each individual layer, weights and outputs after activation function is applied
//todo: add activation function here
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

//allows us to tell what function to use
enum Activationfunctions { Sigmoid = 0, Tanh, LeakyReLU, Elu, Swish, SoftMax, Linear };
static Activationfunctions currentFunc = Sigmoid;

namespace activationFunctions
{
	//holds the ranges if each function
	static glm::vec2 ranges[] = { {0, 1}, {-1, 1}, {0, 0}, {0, 0}, {0, 0}, {0, 1} };

	//each function and its derivitive.
	static float sigmoid(float x) { return 1 / (1.f + std::exp(-x)); }
	static float sigmoidDX(float x) { return sigmoid(x) * (1 - sigmoid(x)); }

	static float tanh(float x) { return 2 / (1 + std::exp(-2 * x)) - 1; }
	static float tanhDX(float x) { return 1 - tanh(x) * tanh(x); }

	static float leakyReLU(float x) { return  __max(x, .01f * x); }
	static float leakyReLUDX(float x) { return (x >= 0) ? 1 : .01; }

	static float elu(float x) { return (x >= 0) ? x : (std::exp(x) - 1); }
	static float eluDX(float x) { return (x >= 0) ? 1.f : (std::exp(x)); }

	static float swish(float x) { return x * sigmoid(x); }
	static float swishDX(float x) { return swish(x) + sigmoid(x) * (1 - swish(x)); }

	//soft max needs to be applied to a vector, so it needs a template
	template <typename T>
	static T softMax(T x) 
	{
		float total = 0;
		for (int i = 0; i < x.length(); i++)
		{
			x[i] = std::exp(x[i]);
			total += x[i];
		}
		return x / total;
	}
	template <typename T>
	static T softMaxDX(T x)
	{
		return x * (T(1.f) - x);
	}

	static float linear(float x) { return x; }
	static float linearDX(float x) { return 1; }
}

struct network
{
	//make a network with no hidden layers
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
	//adds one hidden layer
	void inc()
	{
		hiddenLayersDepth++;
		glm::mat4 tmp = {
			{ randint(4.f), randint(4.f), randint(4.f), randint(4.f) },
			{ randint(4.f), randint(4.f), randint(4.f), randint(4.f) },
			{ randint(4.f), randint(4.f), randint(4.f), randint(4.f) },
			{ randint(4.f), randint(4.f), randint(4.f), randint(4.f) }
		};
		hiddenLayers.emplace_back(tmp);
	}

	//gives network result for given input
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

	//choosing the activation function to use 
	int hiddenLayersDepth = 0;
	static float activationFunc(float x)
	{
		switch (currentFunc)
		{
		case Sigmoid:
			return activationFunctions::sigmoid(x);
			break;
		case Tanh:
			return activationFunctions::tanh(x);
			break;
		case LeakyReLU:
			return activationFunctions::leakyReLU(x);
			break;
		case Elu:
			return activationFunctions::elu(x);
			break;
		case Swish:
			return activationFunctions::swish(x);
			break;
		case SoftMax:
			break;
		case Linear:
			return activationFunctions::linear(x);
		default:
			break;
		}
	}
	static glm::vec2 activationFunc(glm::vec2 x)
	{
		if (currentFunc == SoftMax)
			return activationFunctions::softMax(x);
		x.x = activationFunc(x.x);
		x.y = activationFunc(x.y);
		return x;
	}
	static glm::vec4 activationFunc(glm::vec4 x)
	{
		if (currentFunc == SoftMax)
			return activationFunctions::softMax(x);
		x.x = activationFunc(x.x);
		x.y = activationFunc(x.y);
		x.z = activationFunc(x.z);
		x.w = activationFunc(x.w);
		return x;
	}
	static float activationFuncDX(float x)
	{
		switch (currentFunc)
		{
		case Sigmoid:
			return activationFunctions::sigmoidDX(x);
			break;
		case Tanh:
			return activationFunctions::tanhDX(x);
			break;
		case LeakyReLU:
			return activationFunctions::leakyReLUDX(x);
			break;
		case Elu:
			return activationFunctions::eluDX(x);
			break;
		case Swish:
			return activationFunctions::swishDX(x);
			break;
		case SoftMax:
			break;
		case Linear:
			return activationFunctions::linearDX(x);
		default:
			break;
		}
	}
	static glm::vec2 activationFuncDX(glm::vec2 x)
	{
		if (currentFunc == SoftMax)
			return activationFunctions::softMaxDX(x);
		x.x = activationFuncDX(x.x);
		x.y = activationFuncDX(x.y);
		return x;
	}
	static glm::vec4 activationFuncDX(glm::vec4 x)
	{
		if (currentFunc == SoftMax)
			return activationFunctions::softMaxDX(x);
		x.x = activationFuncDX(x.x);
		x.y = activationFuncDX(x.y);
		x.z = activationFuncDX(x.z);
		x.w = activationFuncDX(x.w);
		return x;
	}//add different activation functions to different layers


	//train network
	void training(glm::vec2 tinput, glm::vec2 expectedOutput)
	{
		//find the difference between output and the expected output
		auto output = input(tinput);
		auto err = expectedOutput - output;
		//if there are no hidden layers, then train the input output func
		if (hiddenLayersDepth == 0)
		{
			//update errors by finding the direction to move to by using the derivative
			err = activationFuncDX(output) * err;
			//move @inputtooutput in the direction of the local min
			inputToOutput += glm::mat2x2(tinput.x * err, tinput.y * err) * learningRate;

			return;
		}
		//the hidden layer errors are found by going back one layer to calculate the error of the previous layer
		auto hiddenErrors = glm::transpose(hiddenToOutput) * err;
		long long layerindex = hiddenLayers.size() - 1;
		{
			//update the errors for the first layer
			err = activationFuncDX(output) * err;

			//move @hiddentooutput towards the derivative minimum
			hiddenToOutput += learningRate * glm::transpose(glm::mat2x4(hiddenLayers[layerindex].outputs * err.x, hiddenLayers[layerindex].outputs * err.y));
		}
		//calculate the error of all the hidden layers(the @hiddenlayers[0].weights doesn't exist since the output is the output of the input layer
		for (; layerindex >= 1; layerindex--)
		{
			//find the layers direction towards the minimum
			auto localmin = activationFuncDX(hiddenLayers[layerindex].outputs) * hiddenErrors;

			//updates the weights towards the minimum
			//the mat4 is because there is no way to transpose the vector to result in a matrix output
			//lr + hiddenlayrers[index].outputs * localmin^T
			hiddenLayers[layerindex].weights += learningRate * glm::transpose(glm::mat4(hiddenLayers[layerindex].outputs * localmin[0], hiddenLayers[layerindex].outputs * localmin[1],
				hiddenLayers[layerindex].outputs * localmin[2], hiddenLayers[layerindex].outputs * localmin[3]));

			//calculate the next hidden layer errors
			hiddenErrors = glm::transpose(hiddenLayers[layerindex - 1].weights) * hiddenErrors;
		}
		{
			//calculate the direction
			auto localmin = activationFuncDX(hiddenLayers[0].outputs) * hiddenErrors;

			//multiply tinput^T * local min
			auto tmp0 = glm::mat2x4(tinput.x * localmin, tinput.y * localmin);
			inputToHidden += learningRate * tmp0;
		}
	}
	//replace the values with random values of 1/sqrt(nodes going into this node)
	void randomizeMatrixies()
	{
		inputToHidden = { {randint(4.f), randint(4.f), randint(4.f), randint(4.f)},
							{randint(4.f), randint(4.f), randint(4.f), randint(4.f)} };
		inputToOutput = { {randint(2.f), randint(2.f)},
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
		//map random int to range (+-)1/sqrt(max)
		return (((float)rand() / (float)RAND_MAX) * 2.f - 1.f) / std::sqrt(max);
	}

	//convert all matrices to string.
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
			for (int i = 1; i < hiddenLayers.size(); i++)
			{
				name += glm::to_string(hiddenLayers[i].weights) + "\n\n\n";
			}

			name += glm::to_string(hiddenToOutput);
		}
		//move each row of the matrix onto its own line
		for (size_t i = name.find("),"); i != std::string::npos; i = name.find("),"))
		{
			name.replace(i, 2, ",\n");
		}
		//separate the size of the matrix from the beginning of the values
		return name.substr(0, name.find("(")) + "\n" + name.substr(name.find("("));
	}

	glm::mat2x4 inputToHidden;
	glm::mat2 inputToOutput;
	glm::mat4x2 hiddenToOutput;
	std::vector<networkLayer> hiddenLayers;

	float learningRate = .002;
};