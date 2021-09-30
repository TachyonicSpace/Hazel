#include "hzpch.h"
#include "ContentBrowserPanel.h"
#include "Hazel/Utils/PlatformUtils.h"
#include "Hazel/Renderer/Texture.h"
#include "Hazel/Scene/Components.h"

#include <imgui/imgui.h>

namespace Hazel {

	// Once we have projects, change this
	extern const std::filesystem::path g_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(g_AssetPath)
	{
	}

	void ContentBrowserPanel::OnImGuiRender(SceneHierarchyPanel& hierarchyPanel)
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			std::string filenameString = relativePath.filename().string();
			ImGui::PushID(filenameString.c_str());			
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));


			if (directoryEntry.is_directory())
			{
				if (ImGui::Button(filenameString.c_str()))
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			else
			{
				if (ImGui::Button(filenameString.c_str()))
				{
					Entity e = hierarchyPanel.GetSelectedEntity();
					if (e != Entity::Null && filenameString.rfind(".png") == filenameString.length() - 4)
					{
						auto& tex = e.GetComponent<Component::SpriteRenderer>();
						tex.Tex = (Texture2D::Create(directoryEntry.path().string()));
					}
				}
			}
			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();


			ImGui::PopID();
		}

		ImGui::End();
	}

}