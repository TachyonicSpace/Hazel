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


	namespace Component
	{
		struct ID
		{
			UUID id;

			ID() = default;
			ID(const ID&) = default;
		};

		struct Tag
		{
			std::string name;

			Tag() = default;
			Tag(const Tag&) = default;
			Tag(const std::string & tag)
				: name(tag) {}
		};

		struct Transform
		{
			glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
			glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
			glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

			Transform() = default;
			Transform(const Transform&) = default;
			Transform(const glm::vec3& pos,
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

		struct SpriteRenderer
		{
			Ref<Texture2D> Tex;
			float TilingFactor;
			Color color{ 1.0f, 1.0f, 1.0f, 1.0f };


			SpriteRenderer(const SpriteRenderer&) = default;
			SpriteRenderer(const Color& col = { 1, 1, 1, 1 }, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
				: color(col), Tex(tex), TilingFactor(tilingFactor)
			{}
			SpriteRenderer(Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& col = { 1, 1, 1, 1 })
				:color(col), Tex(tex), TilingFactor(tilingFactor)
			{}
		};

		struct CircleRenderer
		{
			glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
			float Thickness = 1.0f;
			float Fade = 0.005f;

			CircleRenderer() = default;
			CircleRenderer(const CircleRenderer&) = default;
		};


		struct Cameras
		{
			SceneCamera camera;
			bool Primary = true;

			bool fixedAspectRatio = false;

			Cameras() = default;
			Cameras(const Cameras&) = default;
			Cameras(const SceneCamera& Cam)
				:camera(Cam) {}
		};

		struct NativeScript
		{
			ScriptableEntity* Instance = nullptr;

			ScriptableEntity* (*InstantiateScript)();
			void (*DestroyScript)(NativeScript*);

			template<typename T>
			void Bind()
			{
				InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
				DestroyScript = [](NativeScript* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
			}
		};

		// Physics
		struct Rigidbody2D
		{
			enum class BodyType { Static = 0, Dynamic, Kinematic };
			BodyType Type = BodyType::Static;
			bool FixedRotation = false;

			//initialPhisics State
			glm::vec2 velocity = { 0, 0 };
			float angularVelocity = 0;

			// Storage for runtime
			void* RuntimeBody = nullptr;

			Rigidbody2D() = default;
			Rigidbody2D(const Rigidbody2D&) = default;
		};

		struct BoxCollider2D
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

			BoxCollider2D() = default;
			BoxCollider2D(const BoxCollider2D&) = default;
		};

		struct CircleCollider2D
		{
			glm::vec2 Offset = { 0.0f, 0.0f };
			float Radius = 1;

			// TODO(Yan): move into physics material in the future maybe
			float Density = 1.0f;
			float Friction = 0.5f;
			float Restitution = 0.0f;
			float RestitutionThreshold = 0.5f;

			// Storage for runtime
			void* RuntimeFixture = nullptr;

			CircleCollider2D() = default;
			CircleCollider2D(const CircleCollider2D&) = default;
		};
	}
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
	AddComponentsPopup()


//where to add new render types
Scene::RenderShapes;
Renderer2d
	shader
		vertex array and vertex buffer, index buffer, renderer2d draw function and draw calls in flush.

*/
