#pragma once
#include "Hazel/Core/input.h"

namespace Hazel {

	class HAZEL_API WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int keyCode) override;

		bool IsMouseButtonPressedImpl(int button) override;
		double GetMouseXImpl() override;
		double GetMouseYImpl() override;

		std::pair<double, double> GetMousePosImpl() override;

	};

}