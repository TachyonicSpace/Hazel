#pragma once

#ifdef HZ_PLATFORM_WINDOWS
	
extern Hazel::Application* Hazel::CreateApplication();

#ifdef HZ_MAIN

int main(int argc, char** argv) {

	Hazel::Log::Init();

	HZ_PROFILE_BEGIN_SESSION("startup", "HazelProfile-Startup.json");
	Hazel::CreateApplication();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("runTime", "HazelProfile-runTime.json");
	Hazel::Application::Get().Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("shutdown", "HAzelProfile-shutdown.json");
	HZ_PROFILE_END_SESSION();
}

#endif //HZ_MAIN

#endif // HZ_PLATFORM_WINDOWS
