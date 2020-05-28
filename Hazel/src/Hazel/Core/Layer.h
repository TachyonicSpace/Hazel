#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel\Events\Event.h"
#include "Hazel/Core/Timestep.h"

namespace Hazel {

	class HAZEL_API Layer
	{
	public:
		Layer(const std::string& name = "layer");
		virtual ~Layer();

		virtual void OnAttatch() {}
		virtual void OnDetatch() {}
		virtual void OnUpdate(Timestep& ts)  {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}