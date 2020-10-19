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
	private:
		void DrawEntityNode(Entity node, bool displayAllEntities = false);
		void EditTransformMatrix(glm::mat4& transform, bool details = true);
		void EditColor(Color& col);
	private:
		Ref<Scene> m_context;
		Entity m_SelectedContext;
		static Color* m_SavedColor;
		static short m_SavedColorIndex;
	};

}