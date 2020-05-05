#pragma once

#ifdef HZ_PLATFORM_WINDOWS
	
extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv) {

	Hazel::log::Init();
	HZ_CORE_WARN("initilized log");
	int i = 5;
	HZ_CLIENT_INFO("initilized log Var={0}", i);
	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;

}

#endif // HZ_PLATFORM_WINDOWS
