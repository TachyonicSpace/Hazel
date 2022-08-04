#pragma once

#include <glm/glm.hpp>


#include <glm/gtc/matrix_transform.hpp>

#include <Hazel/Renderer/Color.h>
#include <Hazel/Renderer/Camera.h>
#include "Hazel/Core/UUID.h"
#include <Hazel/Renderer/Texture.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Hazel
{

	// Forward declaration for native script
	class ScriptableEntity;


	
		struct IDComponent
		{
			UUID id;

			IDComponent() = default;
			IDComponent(const IDComponent&) = default;
		};

		struct TagComponent
		{
			std::string name;

			TagComponent() = default;
			TagComponent(const TagComponent&) = default;
			TagComponent(const std::string & tag)
				: name(tag) {}
		};

		struct TransformComponent
		{
			glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
			glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
			glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;
			TransformComponent(const glm::vec3& pos,
					  const glm::vec3& rotation = glm::vec3(0),
					  const glm::vec3& scale = glm::vec3(1))
				: Translation(pos), Rotation(rotation), Scale(scale) {}

			glm::mat4 GetTransform() const
			{
				glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

				return glm::translate(glm::mat4(1.0f), Translation)
					* rotation
					* glm::scale(glm::mat4(1.0f), Scale);
			}

			operator glm::mat4 () { return GetTransform(); }
			operator const glm::mat4& () const { return GetTransform(); }
		};

		struct SpriteRendererComponent
		{
			Ref<Texture2D> Tex;
			float TilingFactor;
			Color color{ 1.0f, 1.0f, 1.0f, 1.0f };


			SpriteRendererComponent(const SpriteRendererComponent&) = default;
			SpriteRendererComponent(const Color& col = { 1, 1, 1, 1 }, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
				: color(col), Tex(tex), TilingFactor(tilingFactor)
			{}
			SpriteRendererComponent(Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& col = { 1, 1, 1, 1 })
				:color(col), Tex(tex), TilingFactor(tilingFactor)
			{}
		};

		struct CircleRendererComponent
		{
			glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
			float Thickness = 1.0f;
			float Fade = 0.005f;

			CircleRendererComponent() = default;
			CircleRendererComponent(const CircleRendererComponent&) = default;
		};


		struct CameraComponent
		{
			SceneCamera camera;
			bool Primary = true;

			bool fixedAspectRatio = false;

			CameraComponent() = default;
			CameraComponent(const CameraComponent&) = default;
			CameraComponent(const SceneCamera& Cam)
				:camera(Cam) {}
		};

		struct NativeScriptComponent
		{
			ScriptableEntity* Instance = nullptr;

			ScriptableEntity* (*InstantiateScript)();
			void (*DestroyScript)(NativeScriptComponent*);

			template<typename T>
			void Bind()
			{
				InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
				DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
			}
		};

		// Physics
		struct Rigidbody2DComponent
		{
			enum class BodyType { Static = 0, Dynamic, Kinematic };
			BodyType Type = BodyType::Static;
			bool FixedRotation = false;

			//initialPhisics State
			glm::vec2 velocity = { 0, 0 };
			float angularVelocity = 0;

			// Storage for runtime
			void* RuntimeBody = nullptr;

			Rigidbody2DComponent() = default;
			Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
		};

		struct BoxCollider2DComponent
		{
			glm::vec2 Offset = { 0.0f, 0.0f };
			glm::vec2 Size = { 0.5f, 0.5f };

			// TODO(Yan): move into physics material in the future maybe
			float Density = 1.0f;
			float Friction = 0.5f;
			float Restitution = 0.0f;
			float RestitutionThreshold = 0.5f;

			// Storage for runtime
			void* RuntimeFixture = nullptr;

			BoxCollider2DComponent() = default;
			BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
		};

		struct CircleCollider2DComponent
		{
			glm::vec2 Offset = { 0.0f, 0.0f };
			float Radius = .5f;

			// TODO(Yan): move into physics material in the future maybe
			float Density = 1.0f;
			float Friction = 0.5f;
			float Restitution = 0.0f;
			float RestitutionThreshold = 0.5f;

			// Storage for runtime
			void* RuntimeFixture = nullptr;

			CircleCollider2DComponent() = default;
			CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
		};
	



	template<typename... Component>
	struct ComponentGroup
	{

	};
	using AllComponents =
		ComponentGroup<TransformComponent, SpriteRendererComponent, CircleRendererComponent,
		CameraComponent, NativeScriptComponent, Rigidbody2DComponent,
		BoxCollider2DComponent, CircleCollider2DComponent>;
}

/*	places to update components at

SceneSeralizer:145 & 305, 

Scene, 
	Copy()
	UpdatePhysics
	updateScripts
	DuplicateEntity()
	OnComponentAdded()

ScenehierarchyPanel
	DrawComponents()


//where to add new render types
Scene::RenderShapes;
Renderer2d
	shader
		vertex array and vertex buffer, index buffer, renderer2d draw function and draw calls in flush.

*/
