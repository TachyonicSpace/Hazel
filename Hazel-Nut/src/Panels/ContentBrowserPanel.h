#pragma once

#include "Hazel/Core/Core.h"
#include <filesystem>

namespace Hazel {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;
	};

}