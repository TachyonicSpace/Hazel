#pragma once


#include "Hazel/Core/Timestep.h"
#include "Hazel/Core/UUID.h"
#include "Hazel/Renderer/Framebuffer.h"

#include <Hazel/Renderer/Camera.h>

class b2World;
#pragma warning(push, 0)
#include "entt.hpp"
#include <glm/glm.hpp>
#pragma warning(pop)

namespace Hazel
{
	class Entity;
	class ScriptableEntity;

	class Scene
	{
	public:
		Scene();
		~Scene()
		{
			HZ_CORE_WARN("destroying scene");
		};

		Entity CreateEntity();
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name, const glm::vec3& transform = glm::vec3(0),
			const glm::vec3& rotation = glm::vec3(0),
			const glm::vec3& scale = glm::vec3(1));
		Entity CreateEntity(const glm::vec3& transform = glm::vec3(0),
							const glm::vec3& rotation = glm::vec3(0),
							const glm::vec3& scale = glm::vec3(1));
		Entity CreateEntity(const std::string& name, const glm::vec3& translation = glm::vec3(0) ,
															const glm::vec3& rotation    = glm::vec3(0) ,
															const glm::vec3& scale       = glm::vec3(1) );

		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		bool OnUpdateRuntime(Timestep& t);
		void OnUpdateEditor(Timestep& ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);
		bool Empty();

		void DrawIDBuffer(Ref<Framebuffer> target, EditorCamera& cam);

		Entity GetPrimaryCameraEntity();

		template <typename ... Args>
		void each(Args&& ... args)
		{
			m_Registry.each(std::forward<Args>(args)...);
		}

	bool ValidEntity(Entity ent);
	bool ValidEntity(std::string str);
	
	public:
		bool ScenePlay = false;
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}