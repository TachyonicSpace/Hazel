#pragma once

#include <Hazel/Core/Core.h>
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"
#include "Hazel/Renderer/Color.h"
namespace Hazel {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender(bool displayAllEntities = false);
		void DrawComponents(Entity ent);

		Entity& GetSelectedEntity() { return m_SelectedContext; }
		Entity GetSelectedEntity() const { return m_SelectedContext; }
		Entity SetSelectedEntity(const Entity& ent) { return (m_SelectedContext = ent); }
		void DeleteSelectedEntity() {
			m_context->DestroyEntity(m_SelectedContext);
			m_SelectedContext = {};
		}
		bool RemoveSelectedEntity();

	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity node, bool displayAllEntities = false);
		void EditTransformMatrix(glm::mat4& transform, bool details = true);
		bool EditTransformVec(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
		void EditColor(Color& col);
	private:
		Ref<Scene> m_context;
		Entity m_SelectedContext;
		static std::vector<Color>& m_SavedColor;
		static size_t m_SavedColorIndex;
	};

}