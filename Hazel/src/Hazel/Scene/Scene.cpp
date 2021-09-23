#include "hzpch.h"
//#include "Scene.h"

#include <Hazel/Scene/Components.h>
#include "Entity.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glad/glad.h>

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

		if (!ValidEntity(name))
			e.AddComponent<Component::Tag>(name);
		else
		{
			int num = -1;
			auto newName = name + "(";
			while (ValidEntity(newName + std::to_string(++num) + ")")) {}

			e.AddComponent<Component::Tag>(newName + std::to_string(num) + ")");
		}

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

	bool Scene::ValidEntity(Entity ent) { return m_Registry.valid((entt::entity)ent); }

	bool Scene::Empty()
	{
		return m_Registry.empty();
	}

	bool Scene::ValidEntity(std::string str)
	{
		bool has = false;
		m_Registry.each([&](auto entity)
			{
				if (m_Registry.has<Component::Tag>(entity))
				{
					Component::Tag t = m_Registry.get<Component::Tag>(entity);
					if (t.name == str)
						has = true;
				}
			}
		);
		return has;
	}

	bool Scene::OnUpdateRuntime(Timestep& t)
	{
		//update scripts
		{
			//on Scene play
			if (ScenePlay)
			{
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
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			auto group = m_Registry.group<Component::Transform>(entt::get<Component::SpriteRenderer>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<Component::Transform, Component::SpriteRenderer>(entity);

				Renderer2D::DrawQuad((uint32_t)entity, transform.GetTransform(), sprite.color, sprite.Tex, sprite.TilingFactor);
			}

			Renderer2D::EndScene();
			return true;
		}
		else
			return false;
	}

	void Scene::OnUpdateEditor(Timestep& ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		auto group = m_Registry.group<Component::Transform>(entt::get<Component::SpriteRenderer>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<Component::Transform, Component::SpriteRenderer>(entity);

			Renderer2D::DrawQuad((uint32_t)entity, transform.GetTransform(), sprite.color, sprite.Tex, sprite.TilingFactor);
		}

		//todo::draw cameras here

		Renderer2D::EndScene();
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

	void Scene::DrawIDBuffer(Ref<Framebuffer> target, EditorCamera& cam)
	{
		target->Bind();

		Renderer2D::BeginScene(cam);

		auto group = m_Registry.group<Component::Transform>(entt::get<Component::SpriteRenderer>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<Component::Transform, Component::SpriteRenderer>(entity);

			Renderer2D::DrawQuad((uint32_t)entity, transform.GetTransform(), sprite.color);
		}

		Renderer2D::EndScene();
	}

	Hazel::Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<Component::Cameras>();
		for (auto ent : view)
		{
			const auto& camera = view.get < Component::Cameras>(ent);
			if (camera.Primary)
				return Entity(ent, this);
		}
		return {};
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
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
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