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
			cleanNonTrainables();

			delete weights;
			delete bias;
			
			delete deltaWeights;
			delete deltaBias;
		}


		void SetInputActivationMatrix(int neuronCount, int batchSize = 1)
		{
			activation = new Matrix(neuronCount, batchSize);
		}
		void SetInputActivationMatrix(Matrix inputMatrix)
		{
			activation = new Matrix(inputMatrix);
		}


		void SetPreviousLayer(NetworkLayer* prevLayer)
		{
			weights = new Matrix(prevLayer->neuronCount, neuronCount);
			weights->Randomize(prevLayer->neuronCount);
			deltaWeights = new Matrix(prevLayer->neuronCount, neuronCount);

			bias = new Matrix(1, neuronCount);
			bias->Randomize(prevLayer->neuronCount);
			deltaBias = new Matrix(1, neuronCount);

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
			cleanNonTrainables();

			activation = new Matrix(batchSize, neuronCount);
			deltaActivation = new Matrix(batchSize, neuronCount);

			sums = new Matrix(batchSize, neuronCount);
			deltaSums = new Matrix(batchSize, neuronCount);

			if (weights != nullptr)
			{
				*sums = (previousLayer->activation)->dot(*weights);
				//sums->ADD_ROW_VEC(bias);
			}

			if(previousLayer != nullptr)
				*activation = sums->ApplyActivationFunction(function);


			if (nextLayer != nullptr)
			{



				return nextLayer->FeedForward();
			}

			if(activation != nullptr)
				return *activation;
		}
		void BackProp(Matrix& Errors)
		{

			Matrix hiddenErrors = this->weights->dot(Errors);//this->weights->dot(Errors);


			Matrix activationError = Errors.Hadamard(activation->ApplyDXFunction(function).T());
			*this->deltaWeights = (activationError).dot(*previousLayer->activation).T() * lr;
			*this->weights = *this->weights - *this->deltaWeights;

			*this->deltaBias = activationError*lr;//same as the delta weights, but we do not multiply by the previous layers output
			*this->bias = *this->bias - *this->deltaBias;

			if (previousLayer->weights != nullptr)
				previousLayer->BackProp(hiddenErrors);
		}

		void cleanNonTrainables()
		{
			if (activation != nullptr)
				delete activation;
			activation = nullptr;
			if (sums != nullptr)
				delete sums;
			sums = nullptr;

			if (deltaActivation != nullptr)
				delete deltaActivation;
			deltaActivation = nullptr;
			if (deltaSums != nullptr)
				delete deltaSums;
			deltaSums = nullptr;
		}

		std::string toString()
		{
			if (weights == nullptr || bias == nullptr)
				return "";

			static std::string funcString[] = { "Sigmoid", "Tanh", "LeakyReLU", "Elu", "Swish", "SoftMax", "Linear" };
			return "<\nweights\n" + weights->toString() + "bias:\n" + bias->toString() + "ActivationFunction: " + funcString[(int)function] + "\n>\n";
		}

		void randomizeMatricies()
		{
			weights->Randomize(previousLayer->neuronCount);
			bias->Randomize(previousLayer->neuronCount);
		}


		Activationfunctions function = Activationfunctions::Tanh;

	private:

		Matrix* activation = nullptr;
		Matrix* sums = nullptr;
		Matrix* weights = nullptr;
		Matrix* bias = nullptr;

		Matrix* deltaActivation = nullptr;
		Matrix* deltaSums = nullptr;
		Matrix* deltaWeights = nullptr;
		Matrix* deltaBias = nullptr;

		int neuronCount;
		float lr;

		NetworkLayer* previousLayer = nullptr;
		NetworkLayer* nextLayer = nullptr;

	};
}