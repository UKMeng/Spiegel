#pragma once

#ifdef SPG_PLATFORM_WINDOWS

extern spg::Application* spg::CreateApplication();

int main(int argc, char** argv) {
	spg::Log::Init();

	SPG_PROFILE_BEGIN_SESSION("Startup", "SpiegelProfile-Startup.json");
	auto app = spg::CreateApplication();
	SPG_PROFILE_END_SESSION();

	SPG_PROFILE_BEGIN_SESSION("Runtime", "SpiegelProfile-Runtime.json");
	app->Run();
	SPG_PROFILE_END_SESSION();

	SPG_PROFILE_BEGIN_SESSION("Shutdown", "SpiegelProfile-Shotdown.json");
	delete app;
	SPG_PROFILE_END_SESSION();
}

#else
	#error Spiegel ONLY support Windows
#endif