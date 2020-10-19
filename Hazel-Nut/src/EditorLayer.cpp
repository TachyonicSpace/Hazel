#include "EditorLayer.h"
#include "imgui/imgui.h"
#include <Hazel/vendor/NumC++/include/NumCpp.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <thread>

namespace Hazel
{
	std::thread* displaying = nullptr;
	std::thread* training = nullptr;
	bool AppStillRunning = true;

	nc::NdArray<float> X = {
			{0, 0},
			{0, 1},
			{.5, .5},
			{1, 0},
			{1, 1}
	};
	nc::NdArray<float> Y =
		nc::NdArray<float>(
			{ 0, 
			  1,
			  .5,
			  1, 
			  0 }).transpose();

	nc::NdArray<float> error = nc::ones<float>(Y.shape());

	uint32_t i = X.row(0).size();
	uint32_t h = 10;
	uint32_t o = Y.row(0).size();

	float lr = .01;
	float reg_param = 0;
	//float mat_iterations = 5000;
	float m = 5;

	nc::NdArray<float> W1 = nc::random::randN<float>({ h, i });
	nc::NdArray<float> W2 = nc::random::randN<float>({ o, h });

	nc::NdArray<float> b1 = nc::zeros<float>({ h, 1 });
	nc::NdArray<float> b2 = nc::zeros<float>({ o, 1 });


	nc::NdArray<float> sigmoid(nc::NdArray<float> x, bool derv = false)
	{
		if (derv)
			return x * (1.f - x);
		nc::NdArray<float> tmp = x;
		for (auto& n : tmp)
			n = 1 / (1 + nc::powerf(glm::e<double>(), -n));
		return tmp;
	}
	std::tuple<nc::NdArray<float>, nc::NdArray<float>, nc::NdArray<float>> forward(nc::NdArray<float> a1)
	{

		auto z2 = nc::dot(W1, a1) + b1;
		auto a2 = sigmoid(z2);

		auto z3 = nc::dot(W2, a2) + b2;
		auto a3 = sigmoid(z3);


		return { a1, a2, a3 };
	}
	nc::NdArray<float> forwardPredict(nc::NdArray<float> a1)
	{
		if (a1.shape().rows == 1)
		{
			auto& [a, b, c] = forward(a1.transpose());
			//HZ_CORE_WARN("{0}", c[0]);
			return c;
		}
		else
		{
			auto& [a, b, c] = forward(a1);
			//HZ_CORE_WARN("{0}", c[0]);
			return c;
		}
	}


	void train(nc::NdArray<float> inputs, nc::NdArray<float> targets)
	{
		nc::NdArray<float> errorLevel = nc::NdArray<float>({ .002, .002, .002, .002, .002 }).transpose();
		auto tmp = nc::less(errorLevel, error);
		bool stillTraining = true;
		for (int i = 0; i < tmp.size(); i++)
		{
			stillTraining |= tmp[i];
		}

		while (AppStillRunning)
		{
			//train(X.row(i).transpose(), Y.row(i));
			if (stillTraining)
			{
				for (int j = 0; j < 20; j++)
				{
					for (int i = 0; i < m; i++)
					{
						auto a1 = inputs.row(i).transpose();
						auto target = targets.row(i);

						auto& [in, hidden, output] = forward(a1);

						auto dZ2 = output - target;
						auto dW2 = nc::dot(dZ2, hidden.transpose()) / m;
						auto db2 = nc::sum(dZ2, nc::Axis::ROW) / m;

						auto dA1 = nc::dot(W2.transpose(), dZ2);
						auto dZ1 = dA1 * (hidden * (1.f - hidden));
						auto dW1 = nc::dot(dZ1, in.transpose()) / m;
						auto db1 = nc::sum(dZ1, nc::Axis::COL) / m;

						W1 -= (dW1 * lr);
						W2 -= lr * dW2;
						b1 -= lr * db1.transpose();
						b2 -= lr * db2;

						//if (j == 0)
							//showNetwork();
					}
				}
			}
			tmp = nc::less(errorLevel, error);
			for (int i = 0; i < tmp.size(); i++)
			{
				stillTraining |= tmp[i];
			}
		}
		HZ_CORE_WARN("training thread ended");
	}
	void display(nc::NdArray<float> inputs, nc::NdArray<float> targets, Ref<Scene> m_Scene)
	{
		while (AppStillRunning)
		{
			m_Scene->m_Registry.each([&](auto entity) {
				if (m_Scene->m_Registry.has<Component::Quads>(entity))
				{
					auto& quad = m_Scene->m_Registry.get<Component::Quads>(entity).q;
					auto& name = m_Scene->m_Registry.get<Component::Tag>(entity).name;

					auto x = std::stof(name.substr(0, name.find_first_of(",")));
					auto y = std::stof(name.substr(name.find_first_of(",") + 2));

					auto val = forwardPredict({ x, y })[0];
					quad.colorRef() = (Color)Color::White * val;
					auto tmp = 0;
				}

				});

		}
		HZ_CORE_WARN("updating thread ended");
	}

	std::stringstream showNetwork()
	{
		//system("CLS");
		//HZ_CORE_WARN("W1:\n\t {0}, {1}\n\t {2}, {3}\n"
		//	"b1\n\t {4},{5}\n"
		//	"w2:\n\t{6}, {7}\n"
		//	"b2\n\t {8}\n\n\n", W1[0, 0], W1[0, 1], W1[1, 0], W1[1, 1],
		//	b1[0], b1[1], W2[0], W2[1], b2[0]);
		std::stringstream ss;
		for (int i = 0; i < m; i++)
		{
			error[i] = forwardPredict(X.row(i))[0];
		}
		error = Y - error;
		ss << "W1: \n\n" << W1 << "B1: \n\n" << b1 << "W2: \n\n" << W2 << "B2: \n\n" << b2 << "\n\nError: \n\n" << error;
		return ss;
	}

	EditorLayer::EditorLayer()
		:Layer("2D sandbox"), m_Camera(1280.f / 720.f, true)
	{
	}
	void EditorLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();

		m_checkerboard = Texture2D::Create("assets/textures/Checkerboard.png");

		FramebufferSpecs fbspec;
		fbspec.Width = 900;
		fbspec.Height = 500;
		m_FrameBuffer = Framebuffer::Create(fbspec);

		m_Scene = NewRef<Scene>();

		m_Delta = .2f;
		auto space = .0f;
		for (float i = 0; i < 1; i += m_Delta)
		{
			for (float j = 0; j < 1; j += m_Delta)
			{
				std::string name = "";
				name = name + std::to_string(i) + ", " + std::to_string(j);
				auto secondSquare = m_Scene->CreateEntity(name);
				Color c = Color::White;
				Quad q({ i * (1 + space), j * (1 + space) }, { m_Delta, m_Delta }, c);
				secondSquare.AddComponent<Component::Quads>(q);
			}
		}

		m_CameraEntity = m_Scene->CreateEntity("first Camera", glm::mat4({ { 1, 0, 0, 0 }, {0, 1, 0, 0}, {0, 0, 1, 0} , {.5, .5, 1.5, 1} }));
		m_CameraEntity.AddComponent<Component::Cameras>(SceneCamera::ProjectionType::Perspective);


		m_SecondCamera = m_Scene->CreateEntity("Second Camera");
		m_SecondCamera.AddComponent<Component::Cameras>().Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate()
			{
				//auto& transform = GetComponent<Component::Transform>().transform;
				//transform[3][0] = rand() % 10 - 5.0f;
			}

			void OnDestroy()
			{
			}

			void OnUpdate(Timestep ts) override
			{
				auto& trans = GetComponent<Component::Transform>().transform;
				float moveSpeed = 5;
				float radianSpeed = .2f;

				if (Input::IsKeyPressed(HZ_KEY_A))
				{
					trans[3][0] -= moveSpeed * ts;
				}

				else if (Input::IsKeyPressed(HZ_KEY_D))
				{
					trans[3][0] += moveSpeed * ts;
				}
				if (Input::IsKeyPressed(HZ_KEY_W))
				{
					trans[3][1] += moveSpeed * ts;
				}
				else if (Input::IsKeyPressed(HZ_KEY_S))
				{
					trans[3][1] -= moveSpeed * ts;
				}
				if (Input::IsKeyPressed(HZ_KEY_Q))
				{
					auto rotate = glm::rotate(glm::mat4(1), radianSpeed += .5f * radianSpeed, { 0, 0, 1 });
					trans = trans * rotate;
				}

				else if (Input::IsKeyPressed(HZ_KEY_E))
				{
					auto rotate = glm::rotate(glm::mat4(1), radianSpeed -= 2 * radianSpeed, { 0, 0, 1 });
					trans = trans * rotate;
				}
			}
		};

		m_CameraEntity.AddComponent<Component::NativeScript>().Bind<CameraController>();
		m_SecondCamera.AddComponent<Component::NativeScript>().Bind<CameraController>();

		m_SceneHierarchyPanel.SetContext(m_Scene);

		training = new std::thread(train, std::ref(X), std::ref(Y));
		displaying = new std::thread(display, std::ref(X), std::ref(Y), std::ref(m_Scene));
	}
	void EditorLayer::OnDetach()
	{
		HZ_PROFILE_FUNCTION();
		AppStillRunning = false;
		displaying->join();
		training->join();
		//displaying->~thread();
		//training->~thread();
	}
	float timeStep;
	void EditorLayer::OnUpdate(Timestep& ts)

	{
		HZ_PROFILE_FUNCTION();

		if (FramebufferSpecs spec = m_FrameBuffer->GetSpecs();
			m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewPortSize.x || spec.Height != m_ViewPortSize.y))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_Camera.OnResize(m_ViewPortSize.x, m_ViewPortSize.y);

			m_Scene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		}

		if (m_ViewPortFocused)
			m_Camera.OnUpdate(ts);


		m_FrameBuffer->Bind();

		RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
		RenderCommand::Clear();


		m_Scene->OnUpdate(ts);

		m_FrameBuffer->UnBind();
		timeStep = ts.GetSeconds();
	}

	void EditorLayer::OnImGuiRender()
	{
		{
			static bool opt_fullscreen_persistant = true;
			bool opt_fullscreen = opt_fullscreen_persistant;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
			// and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
			// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
			// all active windows docked into it will lose their parent and become undocked.
			// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
			// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			bool dockSpaceOpen = true;
			ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// DockSpace
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows,
					// which we can't undo at the moment without finer window depth/z control.
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

					if (ImGui::MenuItem("Exit"))
						Application::Get().Close();

					if (ImGui::MenuItem("Restart"))
					{
						Application::Get().Restart();
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			m_SceneHierarchyPanel.OnImGuiRender();
		}

		ImGui::Begin("Settings");
		ImGui::SliderFloat("angle", &m_angle, 0, 2 * 3.1416f);

		auto& stats = Renderer2D::GetStats();

		ImGui::TextColored({ .8f, .2f, .2f, 1.f }, "frame buffer width: %d", m_FrameBuffer->GetSpecs().Width);
		ImGui::TextColored({ .8f, .2f, .2f, 1.f }, "framebuffer height: %d", m_FrameBuffer->GetSpecs().Height);
		ImGui::TextColored({ .8f, .2f, .8f, 1.f }, "frameRate: %f", 1 / timeStep);

		ImGui::Separator();
		for (int i = 0; i < m; i++)
		{
			std::string label = "targets";
			label += "[" + std::to_string(i) + "]";
			if (ImGui::SliderFloat(label.c_str(), &Y.data()[i], 0, 1))
			{
				AppStillRunning = false;
				displaying->join();
				training->join();

				AppStillRunning = true;
				training = new std::thread(train, std::ref(X), std::ref(Y));
				displaying = new std::thread(display, std::ref(X), std::ref(Y), std::ref(m_Scene));
			}
		}
		std::stringstream network = showNetwork();
		ImGui::Text("%s", network.str().c_str());

		ImGui::Separator();

		ImGui::DragFloat3("Camera Transform",
			glm::value_ptr(m_CameraEntity.GetComponent<Component::Transform>().transform[3]));

		if (ImGui::Checkbox("Camera A", &m_PrimaryCamera))
		{
			m_CameraEntity.GetComponent<Component::Cameras>().Primary = m_PrimaryCamera;
			m_SecondCamera.GetComponent<Component::Cameras>().Primary = !m_PrimaryCamera;
		}
		{
			auto& camera = m_SecondCamera.GetComponent<Component::Cameras>().camera;
			float orthoSize = camera.GetOrthographicSize();
			if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize))
				camera.SetOrthographicSize(orthoSize);
		}


		ImGui::End();


		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			ImGui::Begin("viewPort");

			m_ViewPortFocused = ImGui::IsWindowFocused();
			m_ViewPortHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused || !m_ViewPortHovered);

			auto viewPortSize = ImGui::GetContentRegionAvail();
			m_ViewPortSize = { viewPortSize.x, viewPortSize.y };


			ImGui::Image((void*)(uint64_t)m_FrameBuffer->GetColorAttachmentID(),
				{ m_ViewPortSize.x, m_ViewPortSize.y });
			ImGui::End();
			ImGui::PopStyleVar();
			ImGui::End();
		}
	}
	void EditorLayer::OnEvent(Event& e)
	{
		m_Camera.OnEvent(e);
	}
}