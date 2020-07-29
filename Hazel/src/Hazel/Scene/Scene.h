#pragma once


#include <entt.hpp>

#include "Hazel/Scene/Components.h"

#include "Hazel/Core/Timestep.h"

namespace Hazel
{

	class Scene
	{
	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();

		//tmp
		entt::registry& Reg() { return m_Registry; }

		void OnUpdate(Timestep& t);
	private:
		entt::registry m_Registry;
	};

}