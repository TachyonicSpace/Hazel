#pragma once

#include "glm/glm.hpp"

namespace Hazel {

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float zNear = -1.f, float zFar = 1.f);

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

}