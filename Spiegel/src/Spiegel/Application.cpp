#include "spgpch.h"

#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Log.h"

std::ostream& operator<<(std::ostream& os, const spg::Event& e) {
	return os << e.ToString();
}

namespace spg {
	Application::Application() {

	}

	Application::~Application() {

	}

	void Application::Run() {
		WindowResizeEvent e(1280, 720);
		SPG_CORE_TRACE(e.ToString());
		
		while (true);
	}
}
