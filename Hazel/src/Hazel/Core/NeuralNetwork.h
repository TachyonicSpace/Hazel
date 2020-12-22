/*#pragma once
#include <Hazel/Core/Core.h>
#include <Hazel/Scene/Scene.h>
#include <Hazel/vendor/NumC++/include/NumCpp.hpp>
#include <thread>

namespace Hazel
{

	template<typename T> class NeuralNetwork
	{
	/*public:
		//template<typename T>
		NeuralNetwork(const nc::NdArray<int> sizes, bool normalizingOutputs = true);
		NeuralNetwork(const nc::NdArray<int> sizes, const nc::NdArray<T>& inputs, const nc::NdArray<T>& targets, bool normalizingOutputs = true);
		NeuralNetwork(const NeuralNetwork& nn);


		void training(nc::NdArray<float> inputs = m_inputs, nc::NdArray<float> targets = m_targets);
		void displaying(Ref<Scene> m_Scene, nc::NdArray<float> inputs, nc::NdArray<float> targets);
		void setupScene(Ref<Scene> scene);


		nc::NdArray<T> inputs() { return m_inputs; }
		nc::NdArray<T> inputs(nc::NdArray<T> newInputs) 
		{
			m_inputs = newInputs; 
			if(destroyThread(m_training))
				//make a start training meathod and a perminat training one
		}
		const nc::NdArray<T> inputs() const { return m_inputs; }

		nc::NdArray<T> targets() { return m_targets; }
		const nc::NdArray<T> targets() const { return m_targets; }

		static nc::NdArray<float> sigmoid(nc::NdArray<float> x, bool derv = false)
		{
			if (derv)
				return x * (1.f - x);
			nc::NdArray<float> tmp = x;
			for (auto& n : tmp)
				n = 1 / (1 + nc::powerf(2.718281828459045, -n));
			return tmp;
		}
		std::vector<nc::NdArray<float>> forward(nc::NdArray<float> a1);
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

		bool normalizeOutputs() { return m_normalizeOutputs; }
	private:
		std::thread* m_displaying = nullptr;
		std::thread* m_training = nullptr;
		nc::NdArray<T> m_inputs;
		nc::NdArray<T> m_targets;

		bool StillRunning = true;

		bool destroyThread(std::thread* ptr)
		{
			if (ptr)
			{
				StillRunning = false;
				ptr->join();
				StillRunning = true;
			}
			return ptr;
		}


		float numHiddenLayers;
		std::vector<nc::NdArray<float>> m_Weights;
		std::vector<nc::NdArray<float>> m_biases;
		bool m_normalizeOutputs;
		bool m_initilizedSceneForDisplaying = false;
		
	};
	*//*

	public:
	//template<typename T>
	NeuralNetwork(int inputSize, int hiddenSize, int outputSize, bool normalizingOutputs = true);
	NeuralNetwork(int inputSize, int hiddenSize, int outputSize, const nc::NdArray<T>& inputs, const nc::NdArray<T>& targets, bool normalizingOutputs = true);
	NeuralNetwork(const NeuralNetwork& nn) = default;
	~NeuralNetwork()
	{
		StillRunning = false;
		destroyThread(m_training);
		delete(m_training);
		destroyThread(m_displaying);
		delete(m_displaying);
	}


	void training(nc::NdArray<float> inputs = m_inputs, nc::NdArray<float> targets = m_targets);
	void displaying(Ref<Scene> m_Scene, nc::NdArray<float> inputs, nc::NdArray<float> targets);
	void setupScene(Ref<Scene> scene);


	nc::NdArray<T> inputs() { return m_inputs; }
	nc::NdArray<T> inputs(nc::NdArray<T> newInputs)
	{
		m_inputs = newInputs;
		if(destroyThread(m_training))
			//make a start training meathod and a perminat training one
	}
	const nc::NdArray<T> inputs() const { return m_inputs; }

	nc::NdArray<T> targets() { return m_targets; }
	const nc::NdArray<T> targets() const { return m_targets; }

	static nc::NdArray<float> sigmoid(nc::NdArray<float> x, bool derv = false)
	{
		if (derv)
			return x * (1.f - x);
		nc::NdArray<float> tmp = x;
		for (auto& n : tmp)
			n = 1 / (1 + nc::powerf(2.718281828459045, -n));
		return tmp;
	}
	std::vector<nc::NdArray<float>> forward(nc::NdArray<float> a1);
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

	bool normalizeOutputs() { return m_normalizeOutputs; }
private:
	std::thread* m_displaying = nullptr;
	std::thread* m_training = nullptr;
	nc::NdArray<T> m_inputs;
	nc::NdArray<T> m_targets;

	bool StillRunning = true;

	bool destroyThread(std::thread* ptr)
	{
		if (ptr)
		{
			StillRunning = false;
			ptr->join();
			StillRunning = true;
			return true;
		}
		return false;
	}


	nc::NdArray<float> W1;// = nc::random::randN<float>({ h, i });
	nc::NdArray<float> W2;// = nc::random::randN<float>({ o, h });

	nc::NdArray<float> b1;// = nc::zeros<float>({ h, 1 });
	nc::NdArray<float> b2;// = nc::zeros<float>({ o, 1 });
	bool m_normalizeOutputs;
	bool m_initilizedSceneForDisplaying = false;

};

}*/