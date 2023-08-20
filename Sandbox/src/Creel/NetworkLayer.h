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
			weights = Matrix(prevLayer->neuronCount, neuronCount);
			weights.Randomize(prevLayer->neuronCount);
			deltaWeights = Matrix(prevLayer->neuronCount, neuronCount);

#ifdef useBias
			bias = Matrix(1, neuronCount);
			bias.Randomize(prevLayer->neuronCount);
			deltaBias = Matrix(1, neuronCount);
#endif

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

		Matrix FeedForward(int batchSize = 1)
		{

			activation = Matrix(batchSize, neuronCount);
			deltaActivation = Matrix(batchSize, neuronCount);

			sums = Matrix(batchSize, neuronCount);
			deltaSums = Matrix(batchSize, neuronCount);

			//add bias in prev activation
			sums = (previousLayer->activation/*.addBias()*/).dot(weights);
#ifdef useBias
				sums->ADD_ROW_VEC(bias);
#endif

			if(previousLayer != nullptr)
				activation = sums.ApplyActivationFunction(function);

			if (nextLayer != nullptr)
				return nextLayer->FeedForward(batchSize);

			return activation;
		}
		void BackProp(Matrix& Errors)
		{

			Matrix hiddenErrors = this->weights.dot(Errors);//this->weights->dot(Errors);


			Matrix activationError = Errors.Hadamard(activation.ApplyDXFunction(function).T());
			this->deltaWeights = (activationError).dot(previousLayer->activation).T() * lr;
			this->weights = this->weights - this->deltaWeights;
#ifdef useBias
			*this->deltaBias = activationError*lr;//same as the delta weights, but we do not multiply by the previous layers output
			*this->bias = this->bias->subtractBatches(this->deltaBias->T());
#endif

			if (previousLayer != nullptr && previousLayer->weights.isNotEmpty())
				previousLayer->BackProp(hiddenErrors);
		}

		std::string toString()
		{
#ifdef useBias

			static std::string funcString[] = { "Sigmoid", "Tanh", "LeakyReLU", "Elu", "Swish", "SoftMax", "Linear" };
			return "<\nweights\n" + weights->toString() + "bias:\n" + bias->toString() + "ActivationFunction: " + funcString[(int)function] + "\n>\n";
#else

			static std::string funcString[] = { "Sigmoid", "Tanh", "LeakyReLU", "Elu", "Swish", "SoftMax", "Linear" };
			return "<\nweights\n" + weights.toString() + "ActivationFunction: " + funcString[(int)function] + "\n>\n";
#endif
		}

		void randomizeMatricies()
		{
			weights.Randomize(previousLayer->neuronCount);
#ifdef useBias
			bias.Randomize(previousLayer->neuronCount);
#endif
		}


		Activationfunctions function = Activationfunctions::Tanh;

	private:

		Matrix activation;
		Matrix deltaActivation;

		Matrix weights;
		Matrix deltaWeights;

		Matrix sums;
		Matrix deltaSums;

#ifdef useBias
		Matrix bias;
		Matrix deltaBias;
#endif
		int neuronCount;
		float lr;

		NetworkLayer* previousLayer = nullptr;
		NetworkLayer* nextLayer = nullptr;

	};
}