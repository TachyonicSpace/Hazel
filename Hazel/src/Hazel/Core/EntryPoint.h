#pragma once

#include "Hazel/Core/Core.h"

#ifdef HZ_MAIN

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication(std::string filePath);

int main(int argc, char** argv)
{
	//starts hazel logging system
	Hazel::Log::Init();

	HZ_PROFILE_BEGIN_SESSION("startup", "HazelProfile-Startup.json");
	Hazel::Application* app;
	//passes any files to open to the application
	if (argc > 1)
		app = Hazel::CreateApplication(std::string(argv[1]));
	else
		app = Hazel::CreateApplication(std::string(""));
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("runTime", "HazelProfile-runTime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("shutdown", "HAzelProfile-shutdown.json");
	delete app;
	HZ_PROFILE_END_SESSION();
}

#endif //HZ_MAIN

#endif // HZ_PLATFORM_WINDOWS
