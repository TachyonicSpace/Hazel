#include "hzpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}
namespace Hazel {

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static std::string RigidBody2DBodyTypeToString(Components::Rigidbody2D::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Components::Rigidbody2D::BodyType::Static:    return "Static";
		case Components::Rigidbody2D::BodyType::Dynamic:   return "Dynamic";
		case Components::Rigidbody2D::BodyType::Kinematic: return "Kinematic";
		}

		HZ_CORE_ASSERT(false, "Unknown body type");
		return {};
	}

	static Components::Rigidbody2D::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static")    return Components::Rigidbody2D::BodyType::Static;
		if (bodyTypeString == "Dynamic")   return Components::Rigidbody2D::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return Components::Rigidbody2D::BodyType::Kinematic;

		HZ_CORE_ASSERT(false, "Unknown body type");
		return Components::Rigidbody2D::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		HZ_CORE_ASSERT(entity.HasComponent<Components::ID>());

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID(); // TODO: Entity ID goes here

		if (entity.HasComponent<Components::Tag>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<Components::Tag>().name;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<Components::Transform>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = entity.GetComponent<Components::Transform>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<Components::Cameras>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<Components::Cameras>();
			auto& camera = cameraComponent.camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.fixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.HasComponent<Components::SpriteRenderer>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.GetComponent<Components::SpriteRenderer>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.color.GetVec4();
			out << YAML::Key << "TexturePath" << YAML::Value << spriteRendererComponent.Tex->GetPath();
			out << YAML::Key << "tiling factor" << YAML::Value << spriteRendererComponent.TilingFactor;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<Components::CircleRenderer>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;

			auto& circleRendererComponent = entity.GetComponent<Components::CircleRenderer>();
			out << YAML::Key << "color" << YAML::Value << circleRendererComponent.Color;
			out << YAML::Key << "thickness" << YAML::Value << circleRendererComponent.Thickness;
			out << YAML::Key << "fade" << YAML::Value << circleRendererComponent.Fade;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<Components::Rigidbody2D>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Components::Rigidbody2D

			auto& rb2dComponent = entity.GetComponent<Components::Rigidbody2D>();
			out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;

			out << YAML::Key << "Velocity" << YAML::Value << rb2dComponent.velocity;
			out << YAML::Key << "AngularVelocity" << YAML::Value << rb2dComponent.angularVelocity;

			out << YAML::EndMap; // Components::Rigidbody2D
		}

		if (entity.HasComponent<Components::BoxCollider2D>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // Components::BoxCollider2D

			auto& bc2dComponent = entity.GetComponent<Components::BoxCollider2D>();
			out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;
			out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;

			out << YAML::EndMap; // Components::BoxCollider2D
		}

		if (entity.HasComponent<Components::CircleCollider2D>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;

			auto& cc2dComponent = entity.GetComponent<Components::CircleCollider2D>();
			out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.Offset;
			out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.Radius;
			out << YAML::Key << "Density" << YAML::Value << cc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2dComponent.RestitutionThreshold;

			out << YAML::EndMap;
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		// Not implemented
		HZ_CORE_ASSERT(false);
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return false;
		}
		catch (YAML::BadFile b0)
		{
			return false;
		}
		if (data.IsNull() || !data["Scene"])
			return false;
		try
		{
			std::string sceneName = data["Scene"].as<std::string>();
			HZ_CORE_TRACE("Deserializing scene '{0}'", sceneName);

			auto entities = data["Entities"];
			if (entities)
			{
				for (auto entity : entities)
				{
					uint64_t uuid = entity["Entity"].as<uint64_t>();

					std::string name;
					auto tagComponent = entity["TagComponent"];
					if (tagComponent)
						name = tagComponent["Tag"].as<std::string>();

					HZ_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

					Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

					auto transformComponent = entity["TransformComponent"];
					if (transformComponent)
					{
						// Entities always have transforms
						auto& tc = deserializedEntity.GetComponent<Components::Transform>();
						tc.Translation = transformComponent["Translation"].as<glm::vec3>();
						tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
						tc.Scale = transformComponent["Scale"].as<glm::vec3>();
					}

					auto cameraComponent = entity["CameraComponent"];
					if (cameraComponent)
					{
						auto& cc = deserializedEntity.AddComponent<Components::Cameras>();

						auto& cameraProps = cameraComponent["Camera"];
						cc.camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

						cc.camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
						cc.camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
						cc.camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

						cc.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
						cc.camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
						cc.camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

						cc.Primary = cameraComponent["Primary"].as<bool>();
						cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
					}

					auto spriteRendererComponent = entity["SpriteRendererComponent"];
					if (spriteRendererComponent)
					{
						auto& src = deserializedEntity.AddComponent<Components::SpriteRenderer>();
						src.color = spriteRendererComponent["Color"].as<glm::vec4>();
						if (spriteRendererComponent["TexturePath"])
							src.Tex = Texture2D::Create(spriteRendererComponent["TexturePath"].as<std::string>());

						if (spriteRendererComponent["TilingFactor"])
							src.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
						if (spriteRendererComponent["tiling factor"])
							src.TilingFactor = spriteRendererComponent["tiling factor"].as<float>();
					}

					auto circleRendererComponent = entity["CircleRendererComponent"];
					if (circleRendererComponent)
					{
						auto& crc = deserializedEntity.AddComponent<Components::CircleRenderer>();
						crc.Color = circleRendererComponent["color"].as<glm::vec4>();
						crc.Thickness = circleRendererComponent["thickness"].as<float>();
						crc.Fade = circleRendererComponent["fade"].as<float>();
					}


					auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
					if (rigidbody2DComponent)
					{
						auto& rb2d = deserializedEntity.AddComponent<Components::Rigidbody2D>();
						rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
						rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
						if (rigidbody2DComponent["Velocity"])
						{
							rb2d.velocity = rigidbody2DComponent["Velocity"].as<glm::vec2>();
							rb2d.angularVelocity = rigidbody2DComponent["AngularVelocity"].as<float>();
						}
					}

					auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
					if (boxCollider2DComponent)
					{
						auto& bc2d = deserializedEntity.AddComponent<Components::BoxCollider2D>();
						bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
						bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
						bc2d.Density = boxCollider2DComponent["Density"].as<float>();
						bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
						bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
						bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
					}

					auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
					if (circleCollider2DComponent)
					{
						auto& bc2d = deserializedEntity.AddComponent<Components::CircleCollider2D>();
						bc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
						bc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
						bc2d.Density = circleCollider2DComponent["Density"].as<float>();
						bc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
						bc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
						bc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
					}
				}
			}

			return true;
		}
		catch (YAML::ParserException e)
		{
			HZ_CORE_ERROR("ERROR DECODING SCENE, USING LEGACY DESERIALIZERS, error at {0}", filepath, e.what());
			DeserializeOldScenes(filepath);
		}
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		// Not implemented
		HZ_CORE_ASSERT(false);
		return false;
	}

	bool SceneSerializer::DeserializeOldScenes(const std::string& filepath)
	{

		YAML::Node data = YAML::LoadFile(filepath);
		try
		{


			if (data.IsNull() || !data["Scene"])
				return false;

			std::string sceneName = data["Scene"].as<std::string>();
			HZ_CORE_TRACE("Deserializing scene '{0}'", sceneName);

			auto entities = data["Entities"];
			if (entities)
			{
				for (auto entity : entities)
				{
					uint64_t uuid = entity["Entity"].as<uint64_t>();

					std::string name;
					auto tagComponent = entity["TagComponent"];
					if (tagComponent)
						name = tagComponent["Tag"].as<std::string>();

					HZ_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

					Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

					auto transformComponent = entity["TransformComponent"];
					if (transformComponent)
					{
						// Entities always have transforms
						auto& tc = deserializedEntity.GetComponent<Components::Transform>();
						tc.Translation = transformComponent["Translation"].as<glm::vec3>();
						tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
						tc.Scale = transformComponent["Scale"].as<glm::vec3>();
					}

					auto cameraComponent = entity["CameraComponent"];
					if (cameraComponent)
					{
						auto& cc = deserializedEntity.AddComponent<Components::Cameras>();

						auto& cameraProps = cameraComponent["Camera"];
						cc.camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

						cc.camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
						cc.camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
						cc.camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

						cc.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
						cc.camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
						cc.camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

						cc.Primary = cameraComponent["Primary"].as<bool>();
						cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
					}

					auto spriteRendererComponent = entity["SpriteRendererComponent"];
					if (spriteRendererComponent)
					{
						auto& src = deserializedEntity.AddComponent<Components::SpriteRenderer>();
						src.color = spriteRendererComponent["Color"].as<glm::vec4>();
						if (spriteRendererComponent["tiling factor"])
							src.TilingFactor = spriteRendererComponent["tiling factor"].as<float>();
					}


					auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
					if (rigidbody2DComponent)
					{
						auto& rb2d = deserializedEntity.AddComponent<Components::Rigidbody2D>();
						rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
						rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
					}

					auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
					if (boxCollider2DComponent)
					{
						auto& bc2d = deserializedEntity.AddComponent<Components::BoxCollider2D>();
						bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
						bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
						bc2d.Density = boxCollider2DComponent["Density"].as<float>();
						bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
						bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
						bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
					}

					auto InitialPhysicsStateComponent = entity["InitialPhysicsStateComponent"];
					if (InitialPhysicsStateComponent)
					{
						if (deserializedEntity.HasComponent<Components::Rigidbody2D>())
						{
							auto& ips = deserializedEntity.GetComponent<Components::Rigidbody2D>();
							ips.velocity = InitialPhysicsStateComponent["Velocity"].as<glm::vec2>();
							ips.angularVelocity = InitialPhysicsStateComponent["Angular Velocity"].as<float>();
						}
					}
				}
			}

			return true;
		}
		catch (...)
		{
			try
			{

			}
			catch (...)
			{
				try
				{

				}
				catch (...)
				{
				}
			}
		}
	}

}