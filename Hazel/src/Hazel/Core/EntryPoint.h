#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Core/Application.h"

#ifdef HZ_MAIN

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	//std::error_code PathNotFound;
	//char drive = 'C';
	//auto initPath = std::filesystem::current_path();
	//while (drive != 'Z' + 1)
	//{
	//	std::string newPath = ":\\Hazel\\Hazel-Nut";
	//	newPath = drive + newPath;
	//	std::filesystem::current_path(newPath, PathNotFound);
	//
	//	if (PathNotFound == std::error_code())
	//		break;
	//	PathNotFound = std::error_code();
	//	drive++;
	//}
	//if (drive == 'Z' + 1)
	//	std::filesystem::current_path(initPath);

	//starts hazel logging system
	Hazel::Log::Init();

	HZ_PROFILE_BEGIN_SESSION("startup", "HazelProfile-Startup.json");
	Hazel::Application* app;
	//passes any files to open to the application
		app = Hazel::CreateApplication({ argc, argv });
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
