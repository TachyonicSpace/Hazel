#include "SceneHierarchyPanel.h"

#include "imgui/imgui.h"

#include "Hazel/Scene/Components.h"


namespace Hazel
{

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_context = scene;
	}
	void SceneHierarchyPanel::OnImGuiRender(bool displayAllEntities)
	{
		ImGui::Begin("Scene Hierarchy");

		m_context->m_Registry.each([&](auto entity)
			{
				Entity ent = { entity, m_context.get() };
				DrawEntityNode(ent, displayAllEntities);

			});

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity node, bool displayAllEntities)
	{
		auto& tag = node.GetComponent<Component::Tag>().name;
		if (!displayAllEntities)
			if (tag == "N/A")
				return;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= (m_SelectedContext == node) ? ImGuiTreeNodeFlags_Selected : 0;
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)node, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectedContext = node;
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}
}