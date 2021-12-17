#pragma once

#include "Scene.h"
#include "Hazel/Core/UUID.h"
#include "Components.h"
#pragma warning(push, 0)
#include "entt.hpp"
#pragma warning(pop)
#include "Hazel/Core/log.h"

namespace Hazel
{

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(int handle, Scene* scene);
		Entity(std::string name, entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;
		//~Entity();

		template <typename T, typename ... Args>
		T& AddComponent(Args&& ... args)
		{
			HZ_CORE_ASSERT(!HasComponent<T>(), "already has this component");

			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}
		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
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
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template <typename T>
		T& RemoveComponent()
		{
			auto& item = GetComponent<T>();
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
			return item;
		}

		template <typename T>
		static T& GetComponent(const Scene& scene, entt::entity ent)
		{
			HZ_CORE_ASSERT(HasComponent<T>(scene, ent), "Component not found");
			T tmp = T();
			return (T&)( scene.m_Registry.get<T>(ent));
		}

		template <typename T>
		static bool HasComponent(const Scene& scene, entt::entity ent)
		{
			return scene.m_Registry.all_of<T>(ent);
		}

		template <typename T>
		static T& RemoveComponent(const Scene& scene, entt::entity ent)
		{
			auto item = GetComponent<T>(scene, ent);
			scene.m_Registry.remove<T>(ent);
			return item;
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		UUID GetUUID() { return GetComponent<Components::ID>().id; }		
		const std::string& GetName() { return GetComponent<Components::Tag>().name; }

		bool operator==(const Entity& other) const {
			return
				m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}
		bool operator!=(const Entity& other) const {
			return !operator==(other);
		}

		bool deleted = false;
		
		static Entity Null;

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