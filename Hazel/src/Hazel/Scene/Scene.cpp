#include "hzpch.h"
#include "Scene.h"

#include "Hazel/Scene/Components.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Hazel
{

	Scene::Scene()
	{
	}
	Scene::~Scene()
	{

	}

	entt::entity Scene::CreateEntity()
	{
		return m_Registry.create();
	}

	void Scene::OnUpdate(Timestep& t)
	{
		//here to show everything is linked correctly
		auto test = m_Registry.alive();

		//disable the following line of code to get project to compile and run
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		/*for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.color);
		}*/
	}
}