#include "hzpch.h"
#include "Scene.h"

#include "Components.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Hazel
{

	Scene::Scene()
	{
		#if ENTT_EXAMPLE_CODE
		entt::entity entity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();


		if (m_Registry.has<TransformComponent>(entity))
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);


		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}
		#endif
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