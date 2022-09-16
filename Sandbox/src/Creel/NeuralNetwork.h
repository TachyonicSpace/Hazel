#pragma once

#include <vector>
#include "NetworkLayer.h"
#include "TrainingSet.h"

namespace AI
{

	class FFNeuralNet
	{

	public:
		FFNeuralNet()
		{

		}
		~FFNeuralNet()
		{
			for (NetworkLayer* l : layers)
			{
				delete l;
			}
			layers.clear();
		}

		void Init(int inputNeuronCoiunt, int outputNeuronCount, float lr = .002f)
		{
			if (layers.size() != 0)
				throw "Network already has input layer";

			this->lr = lr;

			NetworkLayer* inputLayer = new NetworkLayer(inputNeuronCoiunt, lr);
			NetworkLayer* outputLayer = new NetworkLayer(outputNeuronCount, lr);

			layers.push_back(inputLayer);
			layers.push_back(outputLayer);
		}

		void AddLayer(int neuronCount)
		{
			if (layers.size() < 1)
				throw "please add an input layer before adding hidden layers, please init network";


			NetworkLayer* hiddenLayer = new NetworkLayer(neuronCount, lr);

			NetworkLayer* outputLayer = layers.back();
			layers.pop_back();
			hiddenLayer->SetPreviousLayer(layers.back());
			layers.push_back(hiddenLayer);

			outputLayer->SetPreviousLayer(layers.back());

			layers.push_back(outputLayer);
			outputLayer->SetNextLayer(nullptr);
		}

		void RemoveNthHiddenLayer(int layerIndex)
		{
			if (layerIndex <= 0 || layerIndex >= layers.size())
				throw "must not remove the first or last layer";

			layers[layerIndex]->removeLayer();
			layers.erase(layers.begin() + layerIndex);
		}

		Matrix ForwardProp(const Matrix inputData, int batchSize = 1)
		{
			if (layers.size() <= 0)
				throw "net cannot feed forward, not enough layers";

			layers[0]->SetInputActivationMatrix(inputData);
			if(layers.size() > 1)
				return layers[1]->FeedForward(batchSize);
			return inputData;
		}

		void BackProp(Matrix& inputData, Matrix& outputData, int batchSize = 1)
		{
			Matrix output = this->ForwardProp(inputData, batchSize);
			Matrix outputError = output - outputData;

			layers.back()->BackProp(outputError.T());
		}

		std::string toString()
		{
			std::string str = "";
			for (NetworkLayer* l : layers)
			{
				str += l->toString() + "\n";
			}
			return str;
		}

		void randomizeMatricies()
		{
			for (int i = 1; i < layers.size(); i++)
			{
				layers[i]->randomizeMatricies();
			}
		}

		float lr;

		std::vector<NetworkLayer*> layers;

	private:
		TrainingSet* trainingSet;
	};


	struct TrainingData
	{
		TrainingData(Matrix in, Matrix target)
			:input(in), output(target)
		{
		}

		Matrix error(FFNeuralNet* net)
		{
			Matrix diff = (output - net->ForwardProp(input));
			return diff.Hadamard(diff);
		}

		void train(FFNeuralNet* nn, float noise = 0)
		{
			nn->BackProp(input, output);
		}
		bool needsTraining(FFNeuralNet* nn, float err = .03f)
		{
			auto errors = error(nn);
			bool traindata = false;
			for (int r = 0; r < errors.shape()[0]; r++)
			{
				for (int c = 0; c < errors.shape()[1]; c++)
				{
					traindata |= (abs(errors(r, c)) > err);
				}
			}
			return traindata;
		}

		Matrix input, output;
	};
}