#pragma once

namespace Hazel {

	//a class for recording the length of time passes between creation and destruction of this class
	class Timestep
	{
	public:
		Timestep(float time = 0)
			:m_Time(time)
		{
		}

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000; }

		operator float() const { return m_Time; }
	//private:
		float m_Time;
	};

}