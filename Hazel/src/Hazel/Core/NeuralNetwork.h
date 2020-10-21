/*#pragma once
#include <Hazel/Core/Core.h>
#include <Hazel/Scene/Scene.h>
#include <Hazel/vendor/NumC++/include/NumCpp.hpp>
#include <thread>

namespace Hazel
{

	template<typename T> class NeuralNetwork
	{
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
		bool m_normalizeOutputs;
		bool m_initilizedSceneForDisplaying = false;
		
	};

}*/