#pragma once



#include "entt.hpp"

#include "Hazel/Core/Timestep.h"

#include <glm/glm.hpp>

namespace Hazel
{
	class Entity;

	class Scene
	{
	public:
		Scene();

		Entity CreateEntity();
		Entity CreateEntity(const glm::mat4& transform);
		Entity CreateEntity(const std::string& name, const glm::mat4& transform = glm::mat4(1));

		void OnUpdate(Timestep& t);
		void OnViewportResize(uint32_t width, uint32_t height);

	//private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};

}