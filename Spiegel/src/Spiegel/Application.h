#pragma once

#include "Core.h"

namespace spg {
	class SPG_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};
	
	// To be defined in CLIENT
	Application* CreateApplication();
}


