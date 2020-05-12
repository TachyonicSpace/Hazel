#include <Hazel.h>

#include "imgui/imgui.h"
#include "glad/glad.h"

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("example")
	{
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);


		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), verticies, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

		glGenBuffers(1, &m_IndexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexArray);

		unsigned int indices[3] = { 0, 1, 2 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("triangle");
		ImGui::SliderFloat2("vertex 1", &verticies[0], -1, 1);
		ImGui::SliderFloat2("vertex 2", &verticies[3], -1, 1);
		ImGui::SliderFloat2("vertex 3", &verticies[6], -1, 1);
		ImGui::End();
	}

	void OnUpdate() override
	{
		//HZ_INFO("ExampleLayer::update");

		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), verticies, GL_DYNAMIC_DRAW);

		glBindVertexArray(m_VertexArray);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

		if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB)) {
			HZ_INFO("tab is pressed");
		}
	}

	void OnEvent(Hazel::Event& event) override
	{
		if (event.GetEventType() == Hazel::EventType::KeyPressed)
		{
			Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;
			HZ_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

private:
	unsigned int m_VertexArray, m_VertexBuffer, m_IndexArray;
	float verticies[3 * 3] =
	{
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

};

class Sandbox : public Hazel::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		//PushOverlay(new Hazel::ImGuiLayer());
	}

	~Sandbox() {

	}
};

Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox;
}