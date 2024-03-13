#pragma once

#ifdef SPG_PLATFORM_WINDOWS

extern spg::Application* spg::CreateApplication();

int main(int argc, char** argv) {
	printf("Spiegel Engine\n");
	auto app = spg::CreateApplication();
	app->Run();
	delete app;
}

#else
	#error Spiegel ONLY support Windows
#endif