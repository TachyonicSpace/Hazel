#include <glm/glm.hpp>
#include <vector>
#include "NeuralNetwork.cpp"

struct TrainingData
{
	TrainingData(glm::vec2 in, glm::vec2 target)
		:input(in), output(target)
	{
	}
	void train(network& nn)
	{
		nn.training(input, output);
	}
	glm::vec2 error(network& nn)
	{
		return output - nn.input(input);
	}
	bool needsTraining(network& nn, float err = .03f)
	{
		auto errors = error(nn);
		return (abs(errors.x) > err) || (abs(errors.y) > err);
	}

	glm::vec2 input;
	glm::vec2 output;
};