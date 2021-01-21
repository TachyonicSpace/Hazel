#define HZ_MAIN
#include <Hazel.h>
#include "imgui/imgui.h"

#include "EditorLayer.h"

namespace Hazel
{

	//creates the hazel game engine editor
	class HazelNut : public Application {
	public:
		HazelNut(std::string filePath)
			:Application(Hazel::WindowProps("Hazelnut Editor", 1500, 700))
		{
			//pushes the file to open on startup into editorLayer
			PushLayer(new EditorLayer(filePath));
		}

		~HazelNut() {

		}
	};

	Application* CreateApplication(std::string filePath) {
		return new HazelNut(filePath);
	}



}