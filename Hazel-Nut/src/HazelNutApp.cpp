#define HZ_MAIN
#include <Hazel.h>
#include "imgui/imgui.h"

#include "EditorLayer.h"

namespace Hazel
{

	//creates the hazel game engine editor
	class HazelNut : public Application {
	public:
		HazelNut(const ApplicationSpecification& specs)
			:Application(specs)//Hazel::WindowProps("Hazelnut Editor", 1200, 560), args)
		{
			//pushes the file to open on startup into editorLayer
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args) {
		ApplicationSpecification spec;
		spec.Name = "Hazelnut Editor";
		spec.WorkingDirectory = "../Hazel-Nut";
		spec.CommandLineArgs = args;
		spec.length = 1200;
		spec.width = 560;
		return new HazelNut(spec);
	}



}