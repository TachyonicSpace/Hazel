#pragma once
#include <armadillo>
#include "ActivationFunctions.h"

#define useBias

namespace AI
{


	static void addOutputBias(arma::mat& m)
	{
#ifdef useBias
		m.insert_cols(m.n_cols, arma::ones<arma::colvec>(m.n_rows));
#endif 

	}
	static void delOutputBias(arma::mat& m, bool flipped = false)
	{
#ifdef useBias
		m.resize(m.n_rows-!flipped, m.n_cols-flipped);
#endif // useBias

	}
	
	
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
			activation = arma::mat(neuronCount, batchSize);
			addOutputBias(activation);
		}
		void SetInputActivationMatrix(arma::mat& inputMatrix)
		{
			activation = inputMatrix;
			addOutputBias(activation);
		}


		void SetPreviousLayer(NetworkLayer* prevLayer)
		{
#ifdef useBias
			weights =      arma::mat(prevLayer->neuronCount+1, neuronCount);
			deltaWeights = arma::mat(prevLayer->neuronCount+1, neuronCount);
#else
			weights = arma::mat(prevLayer->neuronCount, neuronCount);
			deltaWeights = arma::mat(prevLayer->neuronCount, neuronCount);
#endif


			this->previousLayer = prevLayer;
			prevLayer->nextLayer = this;
			randomizeWeights();
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

		arma::mat FeedForward(int batchSize = 1)
		{
			activation = (previousLayer->activation) * (weights);

			if (previousLayer != nullptr) {
				activation = ApplyActivationFunction(&activation);
				if(nextLayer != nullptr)
					addOutputBias(activation);
			}
			if (nextLayer != nullptr)
				return nextLayer->FeedForward(batchSize);

			return activation;
		}



		void BackProp(const arma::mat& Errors)
		{
			arma::mat activationError = Errors % ApplyActivationDXFunction(&activation).t();
			this->deltaWeights = (activationError * previousLayer->activation/*.addBias() */ ).t() * lr;
			this->weights = this->weights - this->deltaWeights;

			if (previousLayer != nullptr && !previousLayer->weights.is_empty()) {
				arma::mat hiddenErrors = this->weights * (Errors);
				delOutputBias(hiddenErrors);
				delOutputBias(previousLayer->activation, true);
				previousLayer->BackProp(hiddenErrors);
			}
		}

		std::string toString(bool useOutputs = false)
		{
			std::ostringstream s;
			if(useOutputs)
				activation.print(s, "<ActivationFunction: " + functionToString[(int)function] + ">\nsize: " + std::to_string((int)activation.n_rows) + ", " + std::to_string((int)activation.n_cols) + "\n");
			else
				weights.print(s, "<ActivationFunction: " + functionToString[(int)function] + ">\nsize: " + std::to_string((int)weights.n_rows) + ", " + std::to_string((int)weights.n_cols) + "\n");
			return s.str();
		}

		void randomizeWeights()
		{
			float mag = 1 / sqrtf(previousLayer->neuronCount);
			weights.for_each([mag](arma::mat::elem_type& el) {el = ((float)rand() / (float)RAND_MAX)*2*mag-mag; });
			/*
			for (int i = 0; i < weights.n_rows; i++)
			{
				for (int j = 0; j < weights.n_cols; j++)
				{
					weights[i, j] = ((float)rand() / (float)RAND_MAX) * 2 * mag - mag;
				}
			}
			*/
		}



		Activationfunctions function = Activationfunctions::Tanh;

	private:

		arma::mat ApplyActivationFunction(arma::mat* mat = nullptr) { 
			if (mat == nullptr)
				mat = &weights;
			arma::mat altered = *mat;
			altered.for_each([this](arma::mat::elem_type& el) {el = activationFunc(el, this->function); }); 
			return altered;
		}
		arma::mat ApplyActivationDXFunction(arma::mat* mat = nullptr) {
			if (mat == nullptr)
				mat = &weights;
			arma::mat altered = *mat;
			altered.for_each([this](arma::mat::elem_type& el) {el = activationFuncDX(el, this->function); });
			return altered;
		}

	private:

		arma::mat activation;

		arma::mat weights;
		arma::mat deltaWeights;

		int neuronCount;
		float lr;

		NetworkLayer* previousLayer = nullptr;
		NetworkLayer* nextLayer = nullptr;

	};
}