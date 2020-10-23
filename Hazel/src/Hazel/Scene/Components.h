#pragma once

#include <glm/glm.hpp>


#include <glm/gtc/matrix_transform.hpp>

#include <Hazel/Renderer/Color.h>
#include <Hazel/Renderer/Camera.h>
#include <Hazel/Renderer/Shape.h>

#include "Entity.h"

namespace Hazel
{

	namespace Component
	{
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
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0 })
					* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
					* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });

				return glm::translate(glm::mat4(1.0f), Translation)
					* rotation
					* glm::scale(glm::mat4(1.0f), Scale);
			}

			operator glm::mat4& () { return GetTransform(); }
			operator const glm::mat4& () const { return GetTransform(); }
		};

		struct SpriteRenderer
		{
			Ref<Texture2D> Tex;
			float TilingFactor;
			Color color{ 1.0f, 1.0f, 1.0f, 1.0f };


			SpriteRenderer(const SpriteRenderer&) = default;
			//SpriteRenderer(const Color& col,                  const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
			//	: color(col), Tex(tex), TilingFactor(tilingFactor)
			//{}
			SpriteRenderer(const Color& col = { 1, 1, 1, 1 }, const Ref<Texture2D>& tex = nullptr, float tilingFactor = 1.f)
				: color(col), Tex(tex), TilingFactor(tilingFactor)
			{}
			SpriteRenderer(Ref<Texture2D>& tex, float tilingFactor = 1.f, const Color& col = { 1, 1, 1, 1 })
				:color(col), Tex(tex), TilingFactor(tilingFactor)
			{}
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

	}
}