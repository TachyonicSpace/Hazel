#define HZ_MAIN
#include <Hazel.h>
#include "imgui/imgui.h"

#include "EditorLayer.h"

namespace Hazel
{


	class HazelNut : public Application {
	public:
		HazelNut()
			:Application({"HazelNut editor"})
		{
			PushLayer(new EditorLayer());
		}

		~HazelNut() {

		}
	};

	Application* CreateApplication() {
		return new HazelNut();
	}



}