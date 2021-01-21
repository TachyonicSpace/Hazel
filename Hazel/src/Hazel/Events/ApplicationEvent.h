#pragma once

#include "Event.h"


namespace Hazel {

	//allows us to handle resising window
	class  WindowResizeEvent : public Event
	{
	public: 
		WindowResizeEvent(unsigned int width, unsigned int height)
			:m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize);
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication);
	private:
		unsigned int m_Width, m_Height;
	};

	//allows us to close window to close application
	class  WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};

	//unknown exactly, assume it handles time within the client app
	class  AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};

	//unknown exactly, assume it handles events within the client app
	class  AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};

	//unknown exactly, assume it handles rendering within the client app
	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};

}