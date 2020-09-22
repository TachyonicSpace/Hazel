#pragma once

#include "Scene.h"
#include "entt.hpp"

namespace Hazel
{

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;
		//~Entity();

		template <typename T, typename ... Args>
		T& AddComponent(Args&& ... args)
		{
			HZ_CORE_ASSERT(!HasComponent<T>(), "already has this component");

			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template <typename T>
		T& GetComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Component not found");

			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template <typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template <typename T>
		T& RemoveComponent()
		{
			auto item = GetComponent<T>();
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
			return item;
		}

		operator bool() const { return m_EntityHandle != entt::null; }
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};

	class ScriptableEntity : public Entity
	{
	public:

		virtual ~ScriptableEntity() = default;
		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}
	protected:
		virtual void OnCreate() {};
		virtual void OnUpdate(Timestep ts) {};
		virtual void OnDestroy() {};

	private:
		Entity m_Entity;
		friend class Scene;
	};

}