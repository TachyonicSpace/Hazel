#pragma once
#include "Hazel/Core/input.h"

namespace Hazel {

	class HAZEL_API WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(KeyCode keyCode) override;

		bool IsMouseButtonPressedImpl(MouseCode button) override;
		float GetMouseXImpl() override;
		float GetMouseYImpl() override;

		std::pair<float, float> GetMousePosImpl() override;

	};

}