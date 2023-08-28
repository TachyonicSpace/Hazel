#define HZ_MAIN
#define _USE_MATH_DEFINES

#define runningProject 1

#if runningProject == 0
	#include "Rockets/RocketProgram.cpp"
	#define layerToAdd RocketLayer
	#else
#if runningProject == 1
	#include "MyAI/FastNN.cpp"
	#define layerToAdd FastNNLayer
#else
#if runningProject == 2
	#include "Creel/CreelNN.cpp"
	#define layerToAdd CreelLayer
	#endif
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
	spec.width = 500;
	spec.length = 500;
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
