#include "ContentBrowserPanel.h"

#include <imgui.h>

namespace spg {

	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_BaseDirectory = "assets";
		m_CurrentDirectory = m_BaseDirectory;
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != m_BaseDirectory) {
			if (ImGui::Button("Back")) {
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		for (auto& directory : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			auto path = directory.path();
			auto filename = path.filename().string();
			if (directory.is_directory()) {
				if (ImGui::Button(filename.c_str())) {
					m_CurrentDirectory = path;
				}
			}
			else {
				if (ImGui::Button(filename.c_str())) { }
			}
		}

		ImGui::End();
	}
}