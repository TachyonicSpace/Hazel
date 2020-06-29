#define HZ_MAIN
#include <Hazel.h>
#include "imgui/imgui.h"

#include "EditorLayer.h"

namespace Hazel
{


	class HazelEditor : public Application {
	public:
		HazelEditor()
			:Application({"Hazel editor"})
		{
			PushLayer(new EditorLayer());
		}

		~HazelEditor() {

		}
	};

	Application* CreateApplication() {
		return new HazelEditor();
	}



}