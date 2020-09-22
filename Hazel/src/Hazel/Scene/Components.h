#pragma once

#include <glm/glm.hpp>
#include <Hazel/Renderer/Color.h>
#include <Hazel/Renderer/Camera.h>

#include "Entity.h"

namespace Hazel
{

	namespace Component
	{
		struct Transform
		{
			glm::mat4 transform{ 1.0f };

			Transform() = default;
			Transform(const Transform&) = default;
			Transform(const glm::mat4& transform)
				: transform(transform) {}

			operator glm::mat4& () { return transform; }
			operator const glm::mat4& () const { return transform; }
		};

		struct SpriteRenderer
		{
			Color color{ 1.0f, 1.0f, 1.0f, 1.0f };

			SpriteRenderer() = default;
			SpriteRenderer(const SpriteRenderer&) = default;
			SpriteRenderer(const Color& color)
				: color(color) {}
		};

		struct Cameras
		{
			SceneCamera camera;
			bool Primary = true;

			bool fixedAspectRatio = false;

			Cameras() = default;
			Cameras(const Cameras&) = default;
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