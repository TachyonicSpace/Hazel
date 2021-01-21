#include "hzpch.h"
#include "Hazel/Core/Window.h"

#ifdef HZ_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Hazel
{
	//creates a new window in the current operating system.
	Scope<Window> Window::Create(const WindowProps& props)
	{
		//only supports windows at this time
		#ifdef HZ_PLATFORM_WINDOWS
			return NewScope<WindowsWindow>(props);
		#else
			HZ_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
		#endif
	}

}