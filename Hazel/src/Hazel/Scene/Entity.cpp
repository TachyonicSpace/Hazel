#include "hzpch.h"
#include "Entity.h"

#include "Components.h"

namespace Hazel
{
	Entity Entity::Null = {};

	Entity::Entity(std::string name, entt::entity handle, Scene* scene)
		:m_EntityHandle(handle), m_Scene(scene)
	{
		AddComponent<Component::Tag>(name);
	}
	Entity::Entity(entt::entity handle, Scene* scene)
		:m_EntityHandle(handle), m_Scene(scene)
	{

	}

	Entity::Entity(int handle, Scene* scene)
		:m_Scene(scene)
	{
		m_EntityHandle = (handle != -1 && scene->m_Registry.valid((entt::entity)handle))? entt::entity(handle) : entt::null;
	}
}