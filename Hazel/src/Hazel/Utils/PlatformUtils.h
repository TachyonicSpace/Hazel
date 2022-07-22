#pragma once

#include <string>

namespace Hazel {

	class FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
		static std::string OpenFolder(std::string initialFolder = "");

	};
	class Time
	{
	public:
		static float GetTime();
	};
}