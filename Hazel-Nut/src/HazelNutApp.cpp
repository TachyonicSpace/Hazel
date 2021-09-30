#define HZ_MAIN
#include <Hazel.h>
#include "imgui/imgui.h"

#include "EditorLayer.h"

namespace Hazel
{

	//creates the hazel game engine editor
	class HazelNut : public Application {
	public:
		HazelNut(ApplicationCommandLineArgs args)
			:Application(Hazel::WindowProps("Hazelnut Editor", 1200, 560), args)
		{
			//pushes the file to open on startup into editorLayer
			PushLayer(new EditorLayer());
		}

		~HazelNut() {

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args) {
		return new HazelNut(args);
	}



}