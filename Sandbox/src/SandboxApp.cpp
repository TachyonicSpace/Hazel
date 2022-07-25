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
	Sandbox(Hazel::ApplicationSpecification specs)
		:Application(specs) {//, 900, 500}) {
		PushLayer(new layerToAdd());
	}

	~Sandbox() {

	}
};

Hazel::Application* Hazel::CreateApplication(Hazel::ApplicationCommandLineArgs args) {
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Sandbox";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
