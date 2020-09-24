#pragma once

#include <Hazel/Core/Core.h>
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"
namespace Hazel {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender(bool displayAllEntities = false);
	private:
		void DrawEntityNode(Entity node, bool displayAllEntities = false);
	private:
		Ref<Scene> m_context;
		Entity m_SelectedContext;
	};

}