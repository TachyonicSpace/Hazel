#include "SceneHierarchyPanel.h"

#include "imgui/imgui.h"

#include "Hazel/Scene/Components.h"


namespace Hazel
{
	Color* SceneHierarchyPanel::m_SavedColor = new Color[16]();
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

		ImGui::End();

		ImGui::Begin("properties");

		if (m_SelectedContext)
		{
			DrawComponents(m_SelectedContext);
		}

		ImGui::End();
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

		if (ent.HasComponent<Component::Transform>() && !ent.HasComponent<Component::Quads>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(Component::Transform).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform Component"))
			{
				auto& transform = ent.GetComponent<Component::Transform>().transform;

				EditTransformMatrix(transform);

				ImGui::TreePop();
			}
		}

		if (ent.HasComponent<Component::Cameras>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(Component::Cameras).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				auto& CameraComp = ent.GetComponent<Component::Cameras>();
				auto& camera = CameraComp.camera;

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
				}

				ImGui::TreePop();
			}
		}

		if (ent.HasComponent<Component::SpriteRenderer>())
		{
			auto& spriteComp = ent.GetComponent<Component::SpriteRenderer>();
			EditColor(spriteComp.color);
		}

		if (ent.HasComponent<Component::Quads>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(Component::Quads).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "quad render info"))
			{
				auto& quad = ent.GetComponent<Component::Quads>().q;
				auto& tex = quad.texRef();
				static char buffer[256];
				//memset(buffer, 0x00, sizeof(buffer));
				ImGui::InputText("Texture address", buffer, 256);
				if (ImGui::Button("submit new texture address"))
					tex = (Texture2D::Create(buffer));

				if (ImGui::TreeNodeEx((void*)typeid(Component::Transform).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform Component"))
				{
					auto& trans = quad.transformRef();
					EditTransformMatrix(trans);

					ImGui::TreePop();
				}
				float& tile = quad.tilingFactorRef();
				ImGui::DragFloat("tiling factor", &tile);
				EditColor(quad.colorRef());


				ImGui::TreePop();
			}
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
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)node, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectedContext = node;
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}
	void SceneHierarchyPanel::EditTransformMatrix(glm::mat4& transform, bool details)
	{
		if (!details)
		{
			ImGui::DragFloat3("Position", &transform[3][0], .1f);
			return;
		}

		static bool degrees = false;

		const char* rotationType[4] = { "Matrix", "Quarterneon", "Axis-angle", "euler angles" };
		static const char* currentRotationType = rotationType[3];

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
		ImGui::DragFloat3("Position", &transform[3][0], .1f);

		//allow edit of the scales in all three directions
		{
			glm::vec3 newScales = oldScales;
			if (ImGui::DragFloat3("Scale", &newScales[0], .1f))
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
			if (ImGui::DragFloat3("axis of rotation", &axis[0], .01f) ||
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
		else if (currentRotationType == "Quarterneon")
		{
			glm::vec4 quarter;
			{
				quarter =
				{
					sqrt((1.0 + (double)rotation[0][0] + rotation[1][1] + rotation[2][2]) / 4.0),
					sqrt((1 + (double)rotation[0][0] - rotation[1][1] - rotation[2][2]) / 4.0),
					sqrt((1 - (double)rotation[0][0] + rotation[1][1] - rotation[2][2]) / 4.0),
					sqrt((1 - (double)rotation[0][0] - rotation[1][1] + rotation[2][2]) / 4.0)
				};
				float largest = __max(__max(__max(quarter[0], quarter[1]), quarter[2]), quarter[3]);

				#define oppositesP(a, b, c) (float)(((double)rotation[a-1][b-1] + rotation[b-1][a-1]) / (4.0 * (double)quarter[c]))
				#define oppositesM(a, b, c) (float)(((double)rotation[a-1][b-1] - rotation[b-1][a-1]) / (4.0 * (double)quarter[c]))

				if (largest == quarter[0])
				{
					quarter[1] = oppositesM(3, 2, 0);
					quarter[2] = oppositesM(1, 3, 0);
					quarter[3] = oppositesM(2, 1, 0);
				}
				else if (largest == quarter[1])
				{
					quarter[0] = oppositesM(3, 2, 1);
					quarter[2] = oppositesP(1, 2, 1);
					quarter[3] = oppositesP(1, 3, 1);
				}
				else if (largest == quarter[2])
				{
					quarter[0] = oppositesM(1, 3, 2);
					quarter[1] = oppositesP(1, 2, 2);
					quarter[3] = oppositesP(2, 3, 2);
				}
				else if (largest == quarter[3])
				{
					quarter[0] = oppositesM(2, 1, 3);
					quarter[1] = oppositesP(1, 3, 3);
					quarter[2] = oppositesP(2, 3, 3);
				}
				#undef oppositesP

				#undef oppositesM

			}
			if
				(ImGui::DragFloat4("quaternion", &quarter[0]))
			{
				{
					auto tmp = quarter[1];
					auto quarter0 = quarter / glm::length(quarter);
					auto mult = [](glm::vec4& q, int a, int b) {return q[a] * q[b] * 2.0f; };
					glm::vec3 r1{ pow(quarter0[0], 2) + pow(quarter0[1], 2) - pow(quarter0[2], 2) - pow(quarter0[3], 2),
								 mult(quarter0, 1, 2) - mult(quarter0, 0, 3), mult(quarter0, 1, 3) + mult(quarter0, 0, 2) };

					glm::vec3 r2{ mult(quarter0, 1, 2) + mult(quarter0, 0, 3),
								  pow(quarter0[0], 2) - pow(quarter0[1], 2) + pow(quarter0[2], 2) - pow(quarter0[3], 2),
								  mult(quarter0, 2, 3) - mult(quarter0, 0, 1) };
					glm::vec3 r3{ mult(quarter0, 1, 3) - mult(quarter0, 0, 2), mult(quarter0, 2, 3) + mult(quarter0, 0, 1),
								  pow(quarter0[0], 2) - pow(quarter0[1], 2) - pow(quarter0[2], 2) + pow(quarter0[3], 2)
					};
					rotation = { r1, r2, r3 };
				}
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
		//maybe add support for more than zyx
		else if (currentRotationType == "euler angles")
		{
			glm::vec3 angles;
			float& z = angles[0], & y = angles[1], & x = angles[2];
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

			if (ImGui::DragFloat3("euler angles", &angles[0], .01f))
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
										{0, cos(angles[2]), -sin(angles[2])},
										{0, sin(angles[2]),  cos(angles[2])} };

				glm::mat3 Y = { {cos(angles[1]),    0,      sin(angles[1])},
										{0,                1,      0, },
										{-sin(angles[1]),   0,      cos(angles[1])} };

				glm::mat3 Z = { {cos(angles[0]), -sin(angles[0]), 0},
								{sin(angles[0]),  cos(angles[0]), 0},
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