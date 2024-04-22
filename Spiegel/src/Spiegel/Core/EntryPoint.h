#pragma once

#ifdef SPG_PLATFORM_WINDOWS

extern spg::Application* spg::CreateApplication();

int main(int argc, char** argv) {
	spg::Log::Init();
	auto app = spg::CreateApplication();;
	app->Run();
	delete app;
}

#else
	#error Spiegel ONLY support Windows
#endif