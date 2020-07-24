#pragma once

#include "Hazel\Core\Layer.h"

#include "Hazel\Events\ApplicationEvent.h"
#include "Hazel\Events\KeyEvent.h"
#include "Hazel\Events\MouseEvent.h"

namespace Hazel
{
	
	class HAZEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttatch() override;
		virtual void OnDetatch() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();
		void SetBlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = false;
		float m_Time = 0.f;
	};

}