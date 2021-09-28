#pragma once

#include "Hazel/Core/Core.h"
#include <filesystem>
#include "SceneHierarchyPanel.h"

namespace Hazel {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender(SceneHierarchyPanel& hierarchyPanel);
	private:
		std::filesystem::path m_CurrentDirectory;
	};

}