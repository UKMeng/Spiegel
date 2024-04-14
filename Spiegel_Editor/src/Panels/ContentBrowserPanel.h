#pragma once

#include "Spiegel.h"
#include <filesystem>

namespace spg {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_BaseDirectory;
	};
}