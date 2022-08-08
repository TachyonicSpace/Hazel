#include "SceneHierarchyPanel.h"

#include "Hazel/Scene/Components.h"
#include "Hazel/Utils/PlatformUtils.h"

#include "Hazel/Scripting/ScriptEngine.h"

#pragma warning(push, 0)
#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>
#pragma warning(pop)

/* The Microsoft C++ compiler is non-compliant with the C++ standard and needs
 * the following definition to disable a security warning on std::strncpy().
 */
#ifdef _MSVC_LANG
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace Hazel
{
	extern const std::filesystem::path g_AssetPath;

	std::vector<Color>& SceneHierarchyPanel::m_SavedColor = Color::ColorPallete;
	size_t SceneHierarchyPanel::m_SavedColorIndex = 0;
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_context = scene;
		m_SelectedContext = {};
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction);

	void SceneHierarchyPanel::OnImGuiRender(bool displayAllEntities)
	{
		//all entities
		if (m_context)
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
			// todo: add physics sprite
			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					m_context->CreateEntity("Empty Entity");


				if (ImGui::MenuItem("Create New Sprite Entity"))
				{
					auto newSprite = m_context->CreateEntity("New Sprite");
					newSprite.AddComponent<SpriteRendererComponent>();
				}

				if (ImGui::MenuItem("Create New Orthographic Camera Entity"))
				{
					auto newCamera = m_context->CreateEntity("New Orthographic Camera");
					newCamera.AddComponent<CameraComponent>();
					newCamera.GetComponent<CameraComponent>().camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
				}

				if (ImGui::MenuItem("Create New Perspective Camera Entity"))
				{
					auto newCamera = m_context->CreateEntity("New Perspective Camera");
					newCamera.AddComponent<CameraComponent>();
					newCamera.GetComponent<CameraComponent>().camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);
				}
				ImGui::EndPopup();
			}


			ImGui::End();

		}

		//properties
		{
			ImGui::Begin("properties");

			if (m_SelectedContext)
			{
				DrawComponents(m_SelectedContext);
			}



			ImGui::End();
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity node, bool displayAllEntities)
	{
		auto tag = node.GetComponent<TagComponent>().name;
		if (!displayAllEntities)
			if (tag == "N/A")
				return;

		tag += "\n\tid:" + std::to_string(node.GetUUID());
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= (m_SelectedContext == node) ? ImGuiTreeNodeFlags_Selected : 0;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)node, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectedContext = node;
		}


		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				node.deleted = true;

			ImGui::EndPopup();
		}



		if (opened)
		{
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, (node.deleted) ? "true" : "false");
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (node.deleted)
		{
			m_context->DestroyEntity(node);
			if (m_SelectedContext == node)
				m_SelectedContext = {};
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity ent)
	{
		if (ent.HasComponent<TagComponent>())
		{
			auto& tag = ent.GetComponent<TagComponent>().name;

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

		// Right-click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Delete entity?"))
				ent.deleted = true;

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<ScriptComponent>("Script");
			DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
			DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
			DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
			DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
			DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", ent, [&](auto& transform)
			{
				EditTransformVec("translation", transform.Translation);
				static bool degrees = true;
				ImGui::Checkbox("degrees?", &degrees);
				if (degrees)
				{
					transform.Rotation = glm::degrees(transform.Rotation);
					EditTransformVec("Rotation", transform.Rotation);
					transform.Rotation = glm::radians(transform.Rotation);
				}
				else
					EditTransformVec("Rotation", transform.Rotation);
				EditTransformVec("Scale", transform.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", ent, [&](auto& CameraComp)
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

		DrawComponent<ScriptComponent>("Script", ent, [](auto& component)
			{
				bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

				static char buffer[64];
				strcpy(buffer, component.ClassName.c_str());

				if (!scriptClassExists)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

				if (ImGui::InputText("Class", buffer, sizeof(buffer)))
					component.ClassName = buffer;

				if (!scriptClassExists)
					ImGui::PopStyleColor();
			});

		DrawComponent<SpriteRendererComponent>("Sprite", ent, [&](auto& src)
			{
				static char buffer[256];
				//memset(buffer, 0x00, sizeof(buffer));

				if (ImGui::InputText("Texture address", buffer, 256))
				{
					try
					{
						std::string defaultFilepath = "D:\\Hazel\\Hazel-Nut\\assets\\textures\\";
						std::string path = buffer;
						if (path.substr(0, defaultFilepath.length()) == defaultFilepath)
							path = path.substr(defaultFilepath.length());
						path = buffer;
						//make create crash optionaly;
						auto tmpTexture = Texture2D::Create(path, true);
						if (tmpTexture != Texture2D::Create())
							src.Tex = tmpTexture;

					}
					catch (...)
					{

					}
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
						Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
						if (texture->IsLoaded())
							src.Tex = texture;
						else
							HZ_WARN("Could not load texture {0}", texturePath.filename().string());
						strcpy(buffer, texturePath.string().c_str());
					}
					ImGui::EndDragDropTarget();
				}

				if (ImGui::Button("Remove Texture"))
				{
					src.Tex = Texture2D::Create();
				}
				float& tile = src.TilingFactor;
				ImGui::DragFloat("tiling factor", &tile);
				EditColor(src.color);


			});

		DrawComponent<CircleRendererComponent>("Circle Renderer", ent, [&](auto& src) {

			ImGui::ColorEdit4("Color", glm::value_ptr(src.Color));
			ImGui::DragFloat("Thickness", &src.Thickness, 0.025f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &src.Fade, 0.00025f, 0.0f, 1.0f);
			
			});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", ent, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 3; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigidbody2DComponent::BodyType)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
				ImGui::DragFloat2("Starting Velocity", glm::value_ptr(component.velocity));
				ImGui::DragFloat("AngularVelocity", &component.angularVelocity);
			});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", ent, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset), .01f);
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size), .01f);
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", ent, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset), .01f);
				ImGui::DragFloat("Radius", &component.Radius, .01f);
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		if (ent.deleted)
		{
			m_context->DestroyEntity(ent);
			if (m_SelectedContext == ent)
				m_SelectedContext = {};
		}
	}


	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectedContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectedContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
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

		//x coordinate
		{
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
		}
		//y coordinate
		{
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
		}
		//z coordinate
		{
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
		}

		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}
	void SceneHierarchyPanel::EditColor(Color& col)
	{
		static int palletLen = int(log2(Color::ColorPallete.size())) + 1;
		ImGui::Text("color pallet length: 2^%d", palletLen);
		ImGui::SameLine();
		ImGui::SliderInt("##x", &palletLen, 0, 8);
		Color::ColorPallete.resize((size_t)pow(2, palletLen));

		if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
		{
			if (m_SavedColorIndex > m_SavedColor.size() - 1)
				m_SavedColorIndex = m_SavedColor.size() - 1;



			ImGui::ColorEdit4("color", col.GetRGBAPointer());
			if (ImGui::Button("invert Color?"))
				col = -col;
			ImGui::SameLine();
			if (ImGui::Button("save color?"))
				m_SavedColor[m_SavedColorIndex] = col;
			ImGui::SameLine();
			if (ImGui::Button("load saved color?"))
				col = m_SavedColor[m_SavedColorIndex];
			ImGui::SliderInt("saved color index", (int*)&m_SavedColorIndex, 0, (int)m_SavedColor.size() - 1);
			ImGui::ColorEdit4("saved color edit", m_SavedColor[m_SavedColorIndex].GetRGBAPointer());

			ImGui::TreePop();
		}
	}
}