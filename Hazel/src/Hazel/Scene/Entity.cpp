#include "hzpch.h"
#include "Entity.h"

#include "Components.h"

namespace Hazel
{
	Entity::Entity(std::string name, entt::entity handle, Scene* scene)
		:m_EntityHandle(handle), m_Scene(scene)
	{
		AddComponent<Component::Tag>(name);
	}
	Entity::Entity(entt::entity handle, Scene* scene)
		:m_EntityHandle(handle), m_Scene(scene)
	{

	}

}