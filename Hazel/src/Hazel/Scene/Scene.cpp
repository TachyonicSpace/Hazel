#include "hzpch.h"
#include "Scene.h"

#include <Hazel/Scene/Components.h>
#include "Entity.h"

#include "Hazel/Renderer/Renderer2D.h"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <glad/glad.h>
#include <glm/glm.hpp>


// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"
#pragma warning(pop)

namespace Hazel
{
	static b2BodyType Rigidbody2DTypeToBox2DBody(Components::Rigidbody2D::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Components::Rigidbody2D::BodyType::Static:    return b2_staticBody;
		case Components::Rigidbody2D::BodyType::Dynamic:   return b2_dynamicBody;
		case Components::Rigidbody2D::BodyType::Kinematic: return b2_kinematicBody;
		}

		HZ_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}


	Scene::Scene()
	{
	}
	template<typename... Components>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
			{

				auto view = src.view<Components>();
				for (auto e : view)
				{
					UUID uuid = src.get<Components::ID>(e).id;
					HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
					entt::entity dstEnttID = enttMap.at(uuid);

					auto& component = src.get<Components>(e);
					dst.emplace_or_replace<Components>(dstEnttID, component);
				}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src,
		const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]() {
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists(dst, src);
	}

	Entity Scene::FindEntity(const UUID& id)
	{
		auto view = m_Registry.view<Components::ID>();
		for (auto e : view)
		{
			UUID uuid = m_Registry.get<Components::ID>(e).id;
			if (uuid != id)
				continue;
			return { e, (Scene*)this };
		}
		return Entity::Null;
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = NewRef<Scene>();

		if (!other)
			return newScene;

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<Components::ID>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<Components::ID>(e).id;
			const auto& name = srcSceneRegistry.get<Components::Tag>(e).name;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components (except IDComponent and TagComponent)
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name, const glm::vec3& translation /* = glm::vec3(0) */,
		const glm::vec3& rotation /* = glm::vec3(0) */,
		const glm::vec3& scale /* = glm::vec3(1) */)
	{
		return CreateEntityWithUUID(UUID(), name, translation, rotation, scale);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name, const glm::vec3& translation /* = glm::vec3(0) */,
		const glm::vec3& rotation /* = glm::vec3(0) */,
		const glm::vec3& scale /* = glm::vec3(1) */)
	{
		Entity e(m_Registry.create(), this);
		e.AddComponent<Components::ID>(uuid);

		e.AddComponent<Components::Tag>(name);

		e.AddComponent<Components::Transform>(translation, rotation, scale);
		return e;
	}
	Entity Scene::CreateEntity(const glm::vec3& translation /* = glm::vec3(0) */,
		const glm::vec3& rotation /* = glm::vec3(0) */,
		const glm::vec3& scale /* = glm::vec3(1) */)
	{
		Entity e(m_Registry.create(), this);
		e.AddComponent<Components::Tag>("N/A");
		e.AddComponent<Components::Transform>(translation, rotation, scale);
		return e;
	}

	Entity Scene::CreateEntity()
	{
		Entity e(m_Registry.create(), this);

		e.AddComponent<Components::Tag>("N/A");
		e.AddComponent<Components::Transform>();
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
				if (m_Registry.all_of<Components::Tag>(entity))
				{
					Components::Tag t = m_Registry.get<Components::Tag>(entity);
					if (t.name == str)
						has = true;
				}
			}
		);
		return has;
	}

	void Scene::OnRuntimeStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Components::Rigidbody2D>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<Components::Transform>();
			auto& rb2d = entity.GetComponent<Components::Rigidbody2D>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;
			bodyDef.linearVelocity.Set(rb2d.velocity.x, rb2d.velocity.y);
			bodyDef.angularVelocity = rb2d.angularVelocity;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<Components::BoxCollider2D>())
			{
				auto& bc2d = entity.GetComponent<Components::BoxCollider2D>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<Components::CircleCollider2D>())
			{
				auto& cc2d = entity.GetComponent<Components::CircleCollider2D>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = transform.Scale.x * cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::RenderShapes(bool renderCameras)
	{
		//draw quads
		{
			auto group = m_Registry.group<Components::Transform>(entt::get<Components::SpriteRenderer>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<Components::Transform, Components::SpriteRenderer>(entity);

				Renderer2D::DrawQuad((uint32_t)entity, transform.GetTransform(), sprite.color, sprite.Tex, sprite.TilingFactor);
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<Components::Transform, Components::CircleRenderer>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<Components::Transform, Components::CircleRenderer>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}


		//todo::draw cameras here
		if (renderCameras)
		{
		}
	}

	void Scene::UpdateScripts(Timestep& t) 
	{
		//on Scene play
		if (ScenePlay)
		{
			m_Registry.view<Components::NativeScript>().each([=](auto entity, auto& nsc)
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

	void Scene::UpdatePhysics(Timestep& t)
	{
		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;
		m_PhysicsWorld->Step(t, velocityIterations, positionIterations);

		// Retrieve transform from Box2D
		auto view = m_Registry.view<Components::Rigidbody2D>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<Components::Transform>();
			auto& rb2d = entity.GetComponent<Components::Rigidbody2D>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			const auto& position = body->GetPosition();
			transform.Translation.x = position.x;
			transform.Translation.y = position.y;
			transform.Rotation.z = body->GetAngle();
		}
	}

	bool Scene::OnUpdateRuntime(Timestep& t)
	{
		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<Components::Transform, Components::Cameras>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<Components::Transform, Components::Cameras>(entity);

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
			
			Scene::RenderShapes(false);
			
			Renderer2D::EndScene();
				return true;
		}
		else
			return false;
	}

	void Scene::OnUpdateEditor(Timestep& ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);
		Scene::RenderShapes(true);
		Renderer2D::EndScene();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<Components::Cameras>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<Components::Cameras>(entity);
			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.SetViewportSize(width, height);
		}

	}

	void Scene::DrawIDBuffer(Ref<Framebuffer> target, EditorCamera& cam)
	{
		target->Bind();

		Renderer2D::BeginScene(cam);

		auto group = m_Registry.group<Components::Transform>(entt::get<Components::SpriteRenderer>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<Components::Transform, Components::SpriteRenderer>(entity);

			Renderer2D::DrawQuad((uint32_t)entity, transform.GetTransform(), sprite.color);
		}

		Renderer2D::EndScene();
	}
	void Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists(AllComponents{}, newEntity, entity);
	}
	Hazel::Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<Components::Cameras>();
		for (auto ent : view)
		{
			const auto& camera = view.get < Components::Cameras>(ent);
			if (camera.Primary)
				return Entity(ent, this);
		}
		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		// static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<Components::ID>(Entity entity, Components::ID& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Components::Transform>(Entity entity, Components::Transform& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Components::Cameras>(Entity entity, Components::Cameras& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<Components::SpriteRenderer>(Entity entity, Components::SpriteRenderer& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Components::CircleRenderer>(Entity entity, Components::CircleRenderer& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Components::Tag>(Entity entity, Components::Tag& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Components::NativeScript>(Entity entity, Components::NativeScript& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Components::Rigidbody2D>(Entity entity, Components::Rigidbody2D& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Components::BoxCollider2D>(Entity entity, Components::BoxCollider2D& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Components::CircleCollider2D>(Entity entity, Components::CircleCollider2D& component)
	{
	}
}