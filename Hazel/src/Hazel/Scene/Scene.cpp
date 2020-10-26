#include "hzpch.h"
//#include "Scene.h"

#include <Hazel/Scene/Components.h>
#include "Entity.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Hazel
{

	Scene::Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name, const glm::vec3& translation /* = glm::vec3(0) */, 
														const glm::vec3& rotation /* = glm::vec3(0) */, 
														const glm::vec3& scale /* = glm::vec3(1) */)
	{
		Entity e(m_Registry.create(), this);
		e.AddComponent<Component::Tag>(name);
		e.AddComponent<Component::Transform>(translation, rotation, scale);
		return e;
	}
	Entity Scene::CreateEntity(const glm::vec3& translation /* = glm::vec3(0) */,
							   const glm::vec3& rotation /* = glm::vec3(0) */,
							   const glm::vec3& scale /* = glm::vec3(1) */)
	{
		Entity e(m_Registry.create(), this);
		e.AddComponent<Component::Tag>("N/A");
		e.AddComponent<Component::Transform>(translation, rotation, scale);
		return e;
	}

	Entity Scene::CreateEntity()
	{
		Entity e(m_Registry.create(), this);

		e.AddComponent<Component::Tag>("N/A");
		e.AddComponent<Component::Transform>();
		return e;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}


	void Scene::OnUpdate(Timestep& t)
	{
		//update scripts
		{
			//on Scene play
			m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& nsc)
				{
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_Entity = { entity, this };
						nsc.Instance->OnCreate();
					}

					nsc.Instance->OnUpdate(t);
				});

		}

		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<Component::Transform, Component::Cameras>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<Component::Transform, Component::Cameras>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

			auto group = m_Registry.group<Component::Transform>(entt::get<Component::SpriteRenderer>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<Component::Transform, Component::SpriteRenderer>(entity);

				Renderer2D::DrawQuad(transform.GetTransform(), sprite.color, sprite.Tex, sprite.TilingFactor);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<Component::Cameras>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<Component::Cameras>(entity);
			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.SetViewportSize(width, height);
		}

	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		//static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<Component::Transform>(Entity entity, Component::Transform& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Component::Cameras>(Entity entity, Component::Cameras& component)
	{
		component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<Component::SpriteRenderer>(Entity entity, Component::SpriteRenderer& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Component::Tag>(Entity entity, Component::Tag& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Component::NativeScript>(Entity entity, Component::NativeScript& component)
	{
	}
}