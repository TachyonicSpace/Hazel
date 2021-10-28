#include <glm/glm.hpp>
#include <vector>

struct network
{
	network::network()
		:hiddenLayersDepth(1), hiddenLayers(0)
	{
		randomizeMatrixies();
	}
	network::network(int depth)
		:hiddenLayersDepth(__max(0, depth - 1)), hiddenLayers(__max(0, depth - 1))
	{
		randomizeMatrixies();
	}

	glm::vec2 input(glm::vec2 inputData)
	{
		if (hiddenLayersDepth == 0)
		{
			return activationFunc(inputToOutput * inputData);
		}

		auto hidden = activationFunc(inputToHidden * inputData);

		for (int i = 0; i < hiddenLayers.size(); i++)
		{
			hidden = activationFunc(hiddenLayers[i] * hidden);
		}

		return activationFunc(hiddenToOutput * hidden);
	}

	int hiddenLayersDepth = 0;
	float activationFunc(float x)
	{
		return 1.f / (1 + std::exp(-x));
	}
	glm::vec2 activationFunc(glm::vec2 x)
	{
		x.x = activationFunc(x.x);
		x.y = activationFunc(x.y);
		return x;
	}
	glm::vec4 activationFunc(glm::vec4 x)
	{
		x.x = activationFunc(x.x);
		x.y = activationFunc(x.y);
		x.z = activationFunc(x.z);
		x.w = activationFunc(x.w);
		return x;
	}
	float iactivationFunc(float x)
	{
		return -std::log(1 / x - 1);
	}
	glm::vec2 iactivationFunc(glm::vec2 x)
	{
		x.x = iactivationFunc(x.x);
		x.y = iactivationFunc(x.y);
		return x;
	}
	glm::vec4 iactivationFunc(glm::vec4 x)
	{
		x.x = iactivationFunc(x.x);
		x.y = iactivationFunc(x.y);
		x.z = iactivationFunc(x.z);
		x.w = iactivationFunc(x.w);
		return x;
	}


	void training(glm::vec2 input, glm::vec2 expectedOutput)
	{

	}
	void randomizeMatrixies()
	{
		inputToHidden = { {randint(), randint(), randint(), randint()},
							{randint(), randint(), randint(), randint()} };
		inputToOutput = { {randint(), randint()}, {
							 randint() , randint() } };
		hiddenToOutput = { {randint(), randint()},
							{randint(), randint()},
							{randint(), randint()},
							{randint(), randint()} };
		for (int i = 0; i < hiddenLayers.size(); i++)
		{
			hiddenLayers[i] = { {randint(), randint(), randint(), randint()},
								{randint(), randint(), randint(), randint()},
								{randint(), randint(), randint(), randint()},
								{randint(), randint(), randint(), randint()} };
		}
	}
	float randint()
	{
		return (float)rand() / (float)RAND_MAX;
	}

	glm::mat2x4 inputToHidden;
	glm::mat2 inputToOutput;
	glm::mat4x2 hiddenToOutput;
	std::vector<glm::mat4> hiddenLayers;
};