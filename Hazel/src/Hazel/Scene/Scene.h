#pragma once


#pragma warning(push, 0)
#include "entt.hpp"
#pragma warning(pop)
#include "Hazel/Core/Timestep.h"
#include "Hazel/Renderer/Framebuffer.h"

#include <Hazel/Renderer/Camera.h>

#include <glm/glm.hpp>

namespace Hazel
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene()
		{
			HZ_CORE_WARN("destroying scene");
		};

		Entity CreateEntity();
		Entity CreateEntity(const glm::vec3& transform = glm::vec3(0),
							const glm::vec3& rotation = glm::vec3(0),
							const glm::vec3& scale = glm::vec3(1));
		Entity Scene::CreateEntity(const std::string& name, const glm::vec3& translation = glm::vec3(0) ,
															const glm::vec3& rotation    = glm::vec3(0) ,
															const glm::vec3& scale       = glm::vec3(1) );

		void DestroyEntity(Entity entity);


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


		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}