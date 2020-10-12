#include "hzpch.h"
//#include "Scene.h"

#include "Components.h"
#include "Entity.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Hazel
{

	Scene::Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name, const glm::mat4& transform /* = glm::mat4(1) */)
	{
		Entity e(m_Registry.create(), this);
		e.AddComponent<Component::Tag>(name);
		e.AddComponent<Component::Transform>(transform);
		return e;
	}
	Entity Scene::CreateEntity(const glm::mat4& transform)
	{
		Entity e(m_Registry.create(), this);
		e.AddComponent<Component::Tag>("N/A");
		e.AddComponent<Component::Transform>(transform);
		return e;
	}

	Entity Scene::CreateEntity()
	{
		Entity e(m_Registry.create(), this);

		e.AddComponent<Component::Tag>("N/A");
		e.AddComponent<Component::Transform>();
		return e;
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
		glm::mat4* cameraTransform = nullptr;
		{
			auto view = m_Registry.view<Component::Transform, Component::Cameras>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<Component::Transform, Component::Cameras>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.camera;
					cameraTransform = &transform.transform;
					break;
				}
			}
		}
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);

			auto group = m_Registry.group<Component::Transform>(entt::get<Component::SpriteRenderer>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<Component::Transform, Component::SpriteRenderer>(entity);

				Renderer2D::DrawQuad(transform, sprite.color);
			}
			auto group0 = m_Registry.group<Component::Quads, Component::Tag>();
			for (auto entity : group0)
			{
				auto& [transform, tag] = group0.get<Component::Quads, Component::Tag>(entity);

				Renderer2D::DrawQuad(transform.q);
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

}