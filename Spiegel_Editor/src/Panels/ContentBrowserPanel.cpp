#include "ContentBrowserPanel.h"

#include <imgui.h>

namespace spg {

	ContentBrowserPanel::ContentBrowserPanel()
		: m_BaseDirectory("assets"), m_CurrentDirectory(m_BaseDirectory)
	{
		m_FolderIcon = Texture2D::Create("assets/icons/folder.png");
		m_FileIcon = Texture2D::Create("assets/icons/file.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.2f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f });

		if (m_CurrentDirectory != m_BaseDirectory) {
			if (ImGui::Button("Back")) {
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directory : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			auto path = directory.path();
			auto filename = path.filename().string();
			ImGui::PushID(filename.c_str());
			
			if (directory.is_directory()) {
				ImGui::ImageButton((ImTextureID)m_FolderIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					m_CurrentDirectory = path;
				}
				
			}
			else {
				ImGui::ImageButton((ImTextureID)m_FileIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::BeginDragDropSource()) {
					const wchar_t* itemPath = path.c_str();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
					ImGui::EndDragDropSource();
				}
			}
			
			ImGui::TextWrapped(filename.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::PopStyleColor(3);
		ImGui::Columns(1);
		ImGui::End();
	}
}