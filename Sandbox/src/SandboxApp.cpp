#define HZ_MAIN
#define _USE_MATH_DEFINES
//#include "RocketProgram.cpp"
#include "NeuralNetworkProgram.cpp"

class Sandbox : public Hazel::Application {
public:
	Sandbox()
		:Application({ "Sandbox" }) {//, 900, 500}) {
		PushLayer(new NeuralNetworkLayer());
	}

	~Sandbox() {

	}
};

Hazel::Application* Hazel::CreateApplication(Hazel::ApplicationCommandLineArgs args) {
	return new Sandbox;
}
