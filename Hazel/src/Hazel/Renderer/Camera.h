#pragma once

#include "glm/glm.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
	//protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float zNear = -1.f, float zFar = 1.f);

		void SetProjection(float left, float right, float bottom, float top, float zNear = -1.f, float zFar = 1.f);

		const glm::vec3& GetPos() const { return m_pos; }
		const float& GetRotation() const { return m_Rotation; }

		void SetPosition(const glm::vec3& pos) { m_pos = pos; RecalculateViewMatrix(); };
		void SetRotation(float zRotation) { m_Rotation = zRotation; RecalculateViewMatrix(); }


		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.f);

		glm::vec3 m_pos = glm::vec3(0);
		float m_Rotation = 0.f;
	};

	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic };
	public:
		SceneCamera(ProjectionType type = ProjectionType::Orthographic);
		virtual ~SceneCamera() = default;


		void SetViewportSize(uint32_t width, uint32_t height);

		int GetProjectionType() const { return (int)m_ProjectionType; }
		void SetProjectionType(int type) { (m_ProjectionType = (ProjectionType)type); RecalculateProjection(); }

		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);

		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		void SetOrthographicNearClip(float i) { (m_OrthographicNear = i);  RecalculateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		void SetOrthographicFarClip(float i) { (m_OrthographicFar = i);  RecalculateProjection(); }

		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveVerticalFOV(float size) { m_PerspectiveFOV = size; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void SetPerspectiveNearClip(float i) { (m_PerspectiveNear = i);  RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		void SetPerspectiveFarClip(float i) { (m_PerspectiveFar = i);  RecalculateProjection(); }

	private:
		void RecalculateProjection();
	private:
		ProjectionType m_ProjectionType;

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		float m_PerspectiveFOV = glm::radians(45.f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.f;

		float m_AspectRatio = 0.0f;
	};

}