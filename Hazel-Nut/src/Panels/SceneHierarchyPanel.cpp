#include "SceneHierarchyPanel.h"

#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>

#include "Hazel/Scene/Components.h"


namespace Hazel
{
	Color* SceneHierarchyPanel::m_SavedColor = (Color*)&Color::White;
	short SceneHierarchyPanel::m_SavedColorIndex = 0;

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

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedContext = {};

		// Right-click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_context->CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}


		ImGui::End();

		ImGui::Begin("properties");

		if (m_SelectedContext)
		{
			DrawComponents(m_SelectedContext);
		}



		ImGui::End();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAllAvailWidth |
			ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(
			);
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity node, bool displayAllEntities)
	{
		auto& tag = node.GetComponent<Component::Tag>().name;
		if (!displayAllEntities)
			if (tag == "N/A")
				return;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= (m_SelectedContext == node) ? ImGuiTreeNodeFlags_Selected : 0;
		flags |= ImGuiTreeNodeFlags_SpanAllAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)node, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectedContext = node;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_context->DestroyEntity(node);
			if (m_SelectedContext == node)
				m_SelectedContext = {};
		}
	}
	void SceneHierarchyPanel::DrawComponents(Entity ent)
	{
		if (ent.HasComponent<Component::Tag>())
		{
			auto& tag = ent.GetComponent<Component::Tag>().name;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("-Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				m_SelectedContext.AddComponent<Component::Cameras>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer"))
			{
				m_SelectedContext.AddComponent<Component::SpriteRenderer>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<Component::Transform>("Transform", ent, [&](auto& transform)
			{
				EditTransformVec("translation", transform.Translation);
				static bool degrees = true;
				if (ImGui::Checkbox("degrees?", &degrees))
				{
					transform.Rotation = glm::degrees(transform.Rotation);
					EditTransformVec("Rotation", transform.Rotation);
					transform.Rotation = glm::radians(transform.Rotation);
				}
				else
					EditTransformVec("Rotation", transform.Rotation);
				EditTransformVec("Scale", transform.Scale, 1.0f);
			});

		DrawComponent<Component::Cameras>("Camera", ent, [&](auto& CameraComp)
			{
				auto& camera = CameraComp.camera;

				ImGui::Checkbox("Primary", &CameraComp.Primary);

				const char* projectionTypeString[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeString[CameraComp.camera.GetProjectionType()];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeString[i];
						if (ImGui::Selectable(projectionTypeString[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeString[i];
							CameraComp.camera.SetProjectionType(i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (CameraComp.camera.GetProjectionType() == (int)SceneCamera::ProjectionType::Perspective)
				{
					float persfarclip = camera.GetPerspectiveFarClip();
					float persnearclip = camera.GetPerspectiveNearClip();
					float perssize = glm::degrees(camera.GetPerspectiveVerticalFOV());

					if (ImGui::DragFloat("vertical FOV", &perssize))
					{
						camera.SetPerspectiveVerticalFOV(glm::radians(perssize));
					}
					if (ImGui::DragFloat("near Clip", &persnearclip))
					{
						camera.SetPerspectiveNearClip(persnearclip);
					}
					if (ImGui::DragFloat("farClip", &persfarclip))
					{
						camera.SetPerspectiveFarClip(persfarclip);
					}
				}
				else if (CameraComp.camera.GetProjectionType() == (int)SceneCamera::ProjectionType::Orthographic)
				{
					float orthofarclip = camera.GetOrthographicFarClip();
					float orthonearclip = camera.GetOrthographicNearClip();
					float orthosize = camera.GetOrthographicSize();

					if (ImGui::DragFloat("size", &orthosize))
					{
						camera.SetOrthographicSize(orthosize);
					}
					if (ImGui::DragFloat("near Clip", &orthonearclip))
					{
						camera.SetOrthographicNearClip(orthonearclip);
					}
					if (ImGui::DragFloat("farClip", &orthofarclip))
					{
						camera.SetOrthographicFarClip(orthofarclip);
					}


					ImGui::Checkbox("Fixed Aspect Ratio", &CameraComp.fixedAspectRatio);
				}


			});

		DrawComponent<Component::SpriteRenderer>("Sprite", ent, [&](auto& src)
			{
				static char buffer[256];
				//memset(buffer, 0x00, sizeof(buffer));
				ImGui::InputText("Texture address", buffer, 256);
				if (ImGui::Button("submit new texture address"))
					src.Tex = (Texture2D::Create(buffer));

				//if (ImGui::TreeNodeEx((void*)typeid(Component::Transform).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform Component"))
				//{
				//	auto& trans = quad.transformRef();
				//	EditTransformMatrix(trans);

				//	ImGui::TreePop();
				//}
				float& tile = src.TilingFactor;
				ImGui::DragFloat("tiling factor", &tile);
				EditColor(src.color);


			});

		/*if (ent.HasComponent<Component::Quads>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			bool open = ImGui::TreeNodeEx((void*)typeid(Component::Quads).hash_code(), treeNodeFlags, "quads");
			ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
			if (ImGui::Button("+", ImVec2{ 20, 20 }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleVar();

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open) {
				auto& quad = ent.GetComponent<Component::Quads>().q;
				auto& tex = quad.texRef();
				static char buffer[256];
				//memset(buffer, 0x00, sizeof(buffer));
				ImGui::InputText("Texture address", buffer, 256);
				if (ImGui::Button("submit new texture address"))
					tex = (Texture2D::Create(buffer));

				//if (ImGui::TreeNodeEx((void*)typeid(Component::Transform).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform Component"))
				//{
				//	auto& trans = quad.transformRef();
				//	EditTransformMatrix(trans);

				//	ImGui::TreePop();
				//}
				float& tile = quad.tilingFactorRef();
				ImGui::DragFloat("tiling factor", &tile);
				EditColor(quad.colorRef());


				ImGui::TreePop();
			}

			if (removeComponent)
				ent.RemoveComponent<Component::Quads>();
		}*/
	}
	void SceneHierarchyPanel::EditTransformMatrix(glm::mat4& transform, bool details)
	{
		if (!details)
		{
			EditTransformVec("Position", (glm::vec3)transform[3]);
			return;
		}

		static bool degrees = false;

		const char* rotationType[3] = { "Matrix", "Axis-angle", "euler angles" };
		static const char* currentRotationType = rotationType[2];

		glm::vec3 oldScales = glm::vec3(glm::length(transform[0]), glm::length(transform[1]), glm::length(transform[2]));

		glm::mat3 rotation;

		//add support for scaling
		if (currentRotationType == "Matrix")
		{
		MATRIX_MODIFYING:

			if (ImGui::BeginCombo("rotation", currentRotationType))
			{
				for (int i = 0; i < 4; i++)
				{
					bool isSelected = currentRotationType == rotationType[i];
					if (ImGui::Selectable(rotationType[i], isSelected))
					{
						currentRotationType = rotationType[i];
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			ImGui::Separator();//finish
			if (ImGui::DragFloat4("0", &transform[0][0], .01f) ||
				ImGui::DragFloat4("1", &transform[1][0], .01f) ||
				ImGui::DragFloat4("2", &transform[2][0], .01f) ||
				ImGui::DragFloat4("3", &transform[3][0], .01f))
			{
				//for (int i = 0; i < 3; i++) {
				//	for (int j = 0; j < 3; j++) {
				//		transform[i][j] = oldScales[i] * rotation[i][j];
				//	}
				//}
			}
			return;
		}

		//display and allow edit of the position in x, y, z;
		EditTransformVec("Position", (glm::vec3&)transform[3]);

		//allow edit of the scales in all three directions
		{
			glm::vec3 newScales = oldScales;
			if (EditTransformVec("Scale", newScales, 1.f))
			{
				for (int i = 0; i < 3; i++)
				{
					if (newScales[i] <= 0)
						newScales[i] = .000001f;

					transform[i] = (transform[i]) * newScales[i] / (oldScales[i]);
				}
			}
		}

		oldScales = glm::vec3(glm::length(transform[0]), glm::length(transform[1]), glm::length(transform[2]));


		ImGui::Separator();
		if (ImGui::BeginCombo("rotation type", currentRotationType))
		{
			for (int i = 0; i < 3; i++)
			{
				bool isSelected = currentRotationType == rotationType[i];
				if (ImGui::Selectable(rotationType[i], isSelected))
				{
					currentRotationType = rotationType[i];
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				rotation[i][j] = transform[i][j] / oldScales[i];
			}
		}
		//oldScales = glm::vec3(glm::length(rotation[0]), glm::length(rotation[1]), glm::length(rotation[2]));

		if (currentRotationType == "Matrix")
			goto MATRIX_MODIFYING;

		else if (currentRotationType == "Axis-angle")
		{
			glm::vec3 axis;
			float theta;
			//calculating axis and angle
			{
				auto d = glm::length(glm::vec3(rotation[2][1] - rotation[1][2],
					rotation[0][2] - rotation[2][0],
					rotation[1][0] - rotation[0][1]));
				float x, y, z;
				if (d != 0)
				{
					x = -(rotation[2][1] - rotation[1][2]) / d;
					y = -(rotation[0][2] - rotation[2][0]) / d;
					z = -(rotation[1][0] - rotation[0][1]) / d;
				}
				else
				{
					double epsilon = 0.0001; // margin to allow for rounding errors
					double epsilon2 = 0.1;

					double xx = (rotation[0][0] + 1) / 2;
					double yy = (rotation[1][1] + 1) / 2;
					double zz = (rotation[2][2] + 1) / 2;
					double xy = (rotation[0][1] + rotation[1][0]) / 4;
					double xz = (rotation[0][2] + rotation[2][0]) / 4;
					double yz = (rotation[1][2] + rotation[2][1]) / 4;
					if ((xx > yy) && (xx > zz)) { // rotation[0][0] is the largest diagonal term
						if (xx < epsilon) {
							x = 0;
							y = 0.7071f;
							z = 0.7071f;
						}
						else {
							x = (float)sqrt(xx);
							y = (float)xy / x;
							z = (float)xz / x;
						}
					}
					else if (yy > zz) { // rotation[1][1] is the largest diagonal term
						if (yy < epsilon) {
							x = (float)0.7071;
							y = (float)0;
							z = (float)0.7071;
						}
						else {
							y = (float)sqrt(yy);
							x = (float)xy / y;
							z = (float)yz / y;
						}
					}
					else { // rotation[2][2] is the largest diagonal term so base result on this
						if (zz < epsilon) {
							x = 0.7071f;
							y = 0.7071f;
							z = 0.f;
						}
						else {
							z = (float)sqrt(zz);
							x = (float)xz / z;
							y = (float)yz / z;
						}
					}
				}
				theta = (float)acos(((double)rotation[0][0] + rotation[1][1] + rotation[2][2] - 1.f) / 2.0f);
				axis = { x, y, z };
			}
			if (degrees)
				theta *= 180 * glm::pi<float>();
			if (EditTransformVec("axis of rotation", axis) ||
				ImGui::DragFloat("rotation", &theta, .001f))
			{
				if (degrees)
					theta /= 180 / glm::pi<float>();
				axis = glm::normalize(axis);
				auto c = cos(theta);
				auto s = sin(theta), t = 1 - c;
				auto x = axis[0], y = axis[1], z = axis[2];

				transform[0][0] = oldScales[0] * (t * x * x + c);
				transform[0][1] = oldScales[0] * (t * x * y + z * s);
				transform[0][2] = oldScales[0] * (t * x * z - y * s);

				transform[1][0] = oldScales[1] * (t * x * y - z * s);
				transform[1][1] = oldScales[1] * (t * y * y + c);
				transform[1][2] = oldScales[1] * (t * y * z + x * s);

				transform[2][0] = oldScales[2] * (t * x * z + y * s);
				transform[2][1] = oldScales[2] * (t * y * z - x * s);
				transform[2][2] = oldScales[2] * (t * z * z + c);
			}
		}
		else if (currentRotationType == "euler angles")
		{
			glm::vec3 angles;
			//float& z = angles[0], & y = angles[1], & x = angles[2];
			float& x = angles[0], & y = angles[1], & z = angles[2];
			ImGui::Checkbox("angles in degrees?", &degrees);

			{
				//float sy = sqrt(rotation[0][0] * rotation[0][0] + rotation[1][0] * rotation[1][0]);


				//bool singular = sy < 1e-3f; // If

				//if (singular)
				//{
				//	X = atan2(-rotation[1][2], rotation[1][1]);
				//	attitude = atan2(-rotation[2][0], sy);
				//	bank = 0;
				//}
				//else
				//{
				//	X = atan2(rotation[2][1], rotation[2][2]);
				//	attitude = atan2(-rotation[2][0], sy);
				//	bank = atan2(rotation[1][0], rotation[0][0]);
				//}




				if (abs(rotation[2][0]) != 1)
				{
					y = __min(-asin(rotation[2][0]), glm::pi<float>() + asin(rotation[2][0]));
					x = atan2(rotation[2][1], rotation[2][2]);
					z = atan2(rotation[1][0], rotation[0][0]);
				}
				else {
					z = 0;
					if (rotation[2][0] == -1)
					{
						y = glm::pi<float>() / 2;
						x = z + atan2(rotation[0][1], rotation[0][2]);
					}
					else
					{
						y = -glm::pi<float>() / 2;
						x = -z + atan2(-rotation[0][1], -rotation[0][2]);
					}
				}

			}

			if (degrees)
				angles *= 180.f / glm::pi<float>();

			if (EditTransformVec("euler angles", angles))
			{
				if (degrees)
					angles /= 180.f / glm::pi<float>();

				//if (x < -1.565f || x > 1.565f)
				//	x = ((x > 0) ? -1 : 1) * 1.568f;
				if (y < -1.565f || y > 1.565f)
					y = ((y > 0) ? -1 : 1) * 1.568f;
				//if (z < -1.565f || z > 1.565f)
				//	z = ((z > 0) ? -1 : 1) * 1.568f;

				glm::mat3 X = { {1,	0,				0},
										{0, cos(x), -sin(x)},
										{0, sin(x),  cos(x)} };

				glm::mat3 Y = { {cos(y),    0,      sin(y)},
										{0,                1,      0, },
										{-sin(y),   0,      cos(y)} };

				glm::mat3 Z = { {cos(z), -sin(z), 0},
								{sin(z),  cos(z), 0},
								{0,				  0,			  1} };

				rotation = X * Y * Z;
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						//float precise = 10E-15f;
						transform[i][j] = oldScales[i] * rotation[i][j];
						//transform[i][j] = ((int)((oldScales[i] * rotation[i][j])/precise))*precise;
					}
				}
			}
		}

		else
		{
			HZ_CORE_ASSERT(false, "INVALID Rotation Type: {0}", currentRotationType);
		}
	}
	bool SceneHierarchyPanel::EditTransformVec(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		bool changed = false;
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}
	void SceneHierarchyPanel::EditColor(Color& col)
	{
		if (ImGui::TreeNodeEx((void*)typeid(Component::SpriteRenderer).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
		{

			ImGui::ColorEdit4("color", col.GetRGBAPointer());
			if (ImGui::Button("invert Color?"))
				col = -col;
			ImGui::SameLine();
			if (ImGui::Button("save color?"))
				m_SavedColor[m_SavedColorIndex] = col;
			ImGui::SameLine();
			if (ImGui::Button("load saved color?"))
				col = m_SavedColor[m_SavedColorIndex];
			ImGui::SliderInt("saved color index", (int*)&m_SavedColorIndex, 0, 15);
			ImGui::ColorEdit4("saved color edit", m_SavedColor[m_SavedColorIndex].GetRGBAPointer());

			ImGui::TreePop();
		}
	}
}