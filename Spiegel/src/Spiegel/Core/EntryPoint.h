#pragma once

#ifdef SPG_PLATFORM_WINDOWS

extern spg::Application* spg::CreateApplication();

int main(int argc, char** argv) {
	spg::Log::Init();
	SPG_CORE_WARN("Initialied Success!");
	int a = 1;
	SPG_INFO("This is Client log. a = {0}", a);


	auto app = spg::CreateApplication();
	app->Run();
	delete app;
}

#else
	#error Spiegel ONLY support Windows
#endif