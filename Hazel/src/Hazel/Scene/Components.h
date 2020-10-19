#pragma once

#include <glm/glm.hpp>
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

		struct Quads
		{
			Quad q;

			Quads() = default;
			Quads(const Quads& q0) = default;
			Quads(const Quad& q0)
				:q(q0) {}
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