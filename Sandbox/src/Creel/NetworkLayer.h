#pragma once
#include "Matrix.h"

namespace AI
{
	class NetworkLayer
	{
	public:
		NetworkLayer(int neuronCounts, float lr)
		{
			this->neuronCount = neuronCounts;
			this->lr = lr;
		}
		~NetworkLayer()
		{
		}


		void SetInputActivationMatrix(int neuronCount, int batchSize = 1)
		{
			activation = Matrix(neuronCount, batchSize);
		}
		void SetInputActivationMatrix(Matrix inputMatrix)
		{
			activation = inputMatrix;
		}


		void SetPreviousLayer(NetworkLayer* prevLayer)
		{
#ifdef useBias
			weights =      Matrix(prevLayer->neuronCount+1, neuronCount);
			deltaWeights = Matrix(prevLayer->neuronCount+1, neuronCount);
#else
			weights = Matrix(prevLayer->neuronCount, neuronCount);
			deltaWeights = Matrix(prevLayer->neuronCount, neuronCount);
#endif
			weights.Randomize(prevLayer->neuronCount);


			this->previousLayer = prevLayer;
			prevLayer->nextLayer = this;
		}
		void SetNextLayer(NetworkLayer* nxtLAyer)
		{
			this->nextLayer = nxtLAyer;
		}

		void removeLayer()
		{
			//remove dependencies from this current layer
			nextLayer->SetPreviousLayer(previousLayer);

			//this->~NetworkLayer();
		}

		Matrix FeedForward(std::string* printResults, int batchSize = 1)
		{
#ifdef useBias
			sums = (previousLayer->activation.addBias()).dot(weights);
#else
			sums = (previousLayer->activation/*.addBias()*/).dot(weights);
#endif

			if (previousLayer != nullptr) {
				activation = sums.ApplyActivationFunction(function);
				if (printResults != nullptr)
				{
#ifdef useBias
					(*printResults) += activation.addBias().toString();
#else
					(*printResults) += activation.toString();
#endif
				}
			}
			if (nextLayer != nullptr)
				return nextLayer->FeedForward(printResults, batchSize);

			return activation;
		}

		Matrix FeedForward(int batchSize = 1) {
			return FeedForward(nullptr, batchSize);
		}

		void BackProp(Matrix& Errors)
		{

			Matrix activationError = Errors.Hadamard(activation.ApplyDXFunction(function).T());
#ifdef useBias
			this->deltaWeights = (activationError).dot(previousLayer->activation.addBias()).T() * lr;
#else
			this->deltaWeights = (activationError).dot(previousLayer->activation).T() * lr;
#endif
			this->weights = this->weights - this->deltaWeights;

			if (previousLayer != nullptr && previousLayer->weights.isNotEmpty()) {
				Matrix hiddenErrors = this->weights.dot(Errors);
				previousLayer->BackProp(hiddenErrors.removeBias());
			}
		}

		std::string toString()
		{
			static std::string funcString[] = { "Sigmoid", "Tanh", "LeakyReLU", "Elu", "Swish", "SoftMax", "Linear" };
			return "<\nweights\n" + weights.toString() + "ActivationFunction: " + funcString[(int)function] + "\n>\n";
		}

		void randomizeMatricies()
		{
			weights.Randomize(previousLayer->neuronCount);
		}


		Activationfunctions function = Activationfunctions::Tanh;

	private:

		Matrix activation;

		Matrix weights;
		Matrix deltaWeights;

		Matrix sums;

		int neuronCount;
		float lr;

		NetworkLayer* previousLayer = nullptr;
		NetworkLayer* nextLayer = nullptr;

	};
}