#pragma once


#pragma warning(push, 0)
#include "entt.hpp"
#pragma warning(pop)
#include "Hazel/Core/Timestep.h"

#include <glm/glm.hpp>

namespace Hazel
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		Entity CreateEntity();
		Entity CreateEntity(const glm::vec3& transform = glm::vec3(0),
							const glm::vec3& rotation = glm::vec3(0),
							const glm::vec3& scale = glm::vec3(1));
		Entity Scene::CreateEntity(const std::string& name, const glm::vec3& translation = glm::vec3(0) ,
															const glm::vec3& rotation    = glm::vec3(0) ,
															const glm::vec3& scale       = glm::vec3(1) );

		void DestroyEntity(Entity entity);

		void OnUpdate(Timestep& t);
		void OnViewportResize(uint32_t width, uint32_t height);

		template <typename ... Args>
		void each(Args&& ... args)
		{
			m_Registry.each(std::forward<Args>(args)...);
		}

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}