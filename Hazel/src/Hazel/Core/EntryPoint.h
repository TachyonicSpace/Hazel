#pragma once

#ifdef HZ_PLATFORM_WINDOWS
	
extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv) {

	Hazel::log::Init();

	HZ_PROFILE_BEGIN_SESSION("startup", "HazelProfile-Startup.json");
	auto app = Hazel::CreateApplication();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("runTime", "HazelProfile-runTime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("shutdown", "HAzelProfile-shutdown.json");
	delete app;
	HZ_PROFILE_END_SESSION();
}

#endif // HZ_PLATFORM_WINDOWS
