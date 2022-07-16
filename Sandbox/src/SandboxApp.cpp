#define HZ_MAIN
#define _USE_MATH_DEFINES

#define runningProject 1

#if runningProject == 0
	#include "RocketProgram.cpp"
	#define layerToAdd RocketLayer
	#else
#if runningProject == 1
	#include "NeuralNetworkProgram.cpp"
	#define layerToAdd NeuralNetworkLayer
	#endif
#endif



class Sandbox : public Hazel::Application {
public:
	Sandbox()
		:Application({ "Sandbox" }) {//, 900, 500}) {
		PushLayer(new layerToAdd());
	}

	~Sandbox() {

	}
};

Hazel::Application* Hazel::CreateApplication(Hazel::ApplicationCommandLineArgs args) {
	return new Sandbox;
}
