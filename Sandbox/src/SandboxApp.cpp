#define HZ_MAIN
#define _USE_MATH_DEFINES
#include <Hazel.h>
#include "Rocket.h"
#include <random>

#include "imgui/imgui.h"



#define WindowWidth 960.0f
#define WindowHeight 540.0f
float rocketW = .065f;
float rocketH = .025f;
float popsize = 1000.f;
float popAverage = 0.f;
vec4 obstacle(0, 0, 1, .2);
vec2 mouse;
int numThreads = std::thread::hardware_concurrency();

int testing = -1;


bool restart = false;

struct population {
	std::vector<Rocket> rocketPopulation;
	std::vector<Rocket> matingPool;

	float prevPopSize;

	population(float width, float height) {
		Rocket::count = 0;
		for (int i = 0; i < popsize; i++) {
			rocketPopulation.push_back(*(new Rocket(width, height)));
		}
		prevPopSize = popsize;
	}

	void pos(glm::vec2 pos) {
		//todo: multithread this
		for (Rocket r : rocketPopulation) {
			r.Pos = pos;
		}
	}

	float eval() {
		float average = 0;
		static float previous[5] = { 0, 0, 0, 0, 0 };
		for (int i = 0; i < 4; i++)
		{
			previous[i] = previous[i + 1];
		}
		//todo: multithread this
		for (int i = 0; i < rocketPopulation.size(); i++) {
			average = __max(rocketPopulation[i].fitness(&obstacle), average);
		}
		previous[4] = average;
		matingPool.resize(0);

		//todo: multithread this
		for (int i = 0; i < rocketPopulation.size(); i++) {
			float fitness = rocketPopulation[i].Fitness;
			auto n = clamp(fitness, 1.f, 1000.f);
			for (int j = 0; j < n; j++) {
				matingPool.push_back(rocketPopulation[i]);
			}
		}

		selection();
		matingPool.resize(0);

		//if all the populations in memory are the same average fitness, then create a random rocket
		bool same = true;
		for (float f : previous)
			same &= f == previous[0];
		if (same)
		{
			rocketPopulation[0] = Rocket(rocketW, rocketH);
		}

		return average;
	}

	void selection() {

		rocketPopulation.resize((size_t)popsize);
		//todo: multithread this
		for (size_t i = 0; i < rocketPopulation.size(); i++) {
			auto index = Rand((matingPool.size() - 1), 0);
			auto partnerA = (matingPool[(size_t)index]);

			index = Rand((matingPool.size() - 1), 0);
			auto partnerB = (matingPool[(size_t)index]);

			auto child = partnerA.crossover(partnerB);

			this->rocketPopulation[i] = child;
		}
		prevPopSize = popsize;
	}


};


class Sandbox2D : public Hazel::Layer
{
public:
	Sandbox2D()
		:Layer("2D sandbox"), m_Camera(Hazel::Application::Get().GetWindow().GetAspectRatio())
	{
	}

	~Sandbox2D() = default;

	void OnAttach()
	{
		HZ_PROFILE_FUNCTION();

		//Hazel::Application::Get().GetWindow().SetVSync(false);
	}

	void OnDetach()
	{
		HZ_PROFILE_FUNCTION();

	}

	void OnUpdate(Hazel::Timestep& ts)
	{
		Hazel::RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
		Hazel::RenderCommand::Clear();

		if (restart)
		{
			pop = *(new population(rocketW, rocketH));
			restart = false;
		}
		Hazel::Renderer2D::ResetStats();
		Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());

		bool notFinished = false;

		{
			for (int i = 0; i < pop.rocketPopulation.size(); i++)
			{

				Rocket& r = pop.rocketPopulation[i];

				glm::mat4 transform;
				glm::vec2 size = { rocketW, rocketH };
				if (r.angle == 0)
				{
					transform = glm::translate(glm::mat4(1), r.pos()) *
						glm::scale(glm::mat4(1), { size.x, size.y, 1 });
				}
				else
				{
					transform = glm::translate(glm::mat4(1), r.pos()) *
						glm::rotate(glm::mat4(1), r.angle, { 0, 0, 1 }) *
						glm::scale(glm::mat4(1), { size.x, size.y, 1 });
				}

				Hazel::Renderer2D::DrawQuad(transform, r.col);


				if (!notPaused)
				{
					notFinished |= r.update(&obstacle);
				}
			}
			if (!notPaused)
			{
				Rocket::count++;
				if (!notFinished)
					goto Finished;
			}
			if (Rocket::count >= DNA::lifespan) {
			Finished:
				Rocket::count = 0;
				//Rocket::targetSize *= (Rocket::targetSize < .005) ? 1 : .9f;
				pop.eval();
			}
		}

		{
			Hazel::Renderer2D::DrawQuad(Rocket::rocketTarget, { Rocket::targetSize, Rocket::targetSize }, { 0, 1, 1, .8f });

			Hazel::Renderer2D::DrawQuad({ obstacle.x, obstacle.y, -.8 }, { obstacle.z, obstacle.w }, { 1, 0, .8f });

		}

		Hazel::Renderer2D::EndScene();
		timeStep = ts.GetSeconds();
	}


	void OnImGuiRender()
	{


		//settings
		{
			ImGui::Begin("Settings");

#if 1
			ImGui::Checkbox("restart", &restart); ImGui::SameLine();
			ImGui::Checkbox("PauseRockets?", &notPaused);

			ImGui::SliderInt("index", &testing, -1, (int)pop.rocketPopulation.size() - 1);
			ImGui::SliderFloat("popsize", &popsize, 1, 1000);

			static auto& window = Hazel::Application::Get().GetWindow();
			static float frametime = 1.f;
			if (testing == -1)
			{
				static Rocket rocket = pop.rocketPopulation[0];
				rocket.Pos = { Hazel::Input::GetMouseX() / window.GetWidth() * 2.f * window.GetAspectRatio() - window.GetAspectRatio(), -(Hazel::Input::GetMouseY() / window.GetHeight() * 2.f - 1) };
				ImGui::Text("pos: %f, %f", rocket.Pos.x, rocket.Pos.y);
				ImGui::Text("vel: x~%f, y~%f", rocket.vel.x, rocket.vel.y);
				ImGui::Text("Life left: %d", DNA::lifespan - Rocket::count);
				ImGui::ColorEdit4("color:", rocket.col.GetRGBAPointer());
				ImGui::SliderFloat("mutationRate", &Rocket::mutationRate, 0, 1);
				ImGui::SliderFloat("mutationPow", &Rocket::mutationpow, -1, 2);
				rocket.angle += .5f;
				rocket.angle = fmod(rocket.angle, 3.141592f * 2.0f);
				ImGui::Text("fitness: %f", rocket.fitness(&obstacle));

				Hazel::Renderer2D::BeginScene(m_Camera.GetCamera());
				Hazel::Renderer2D::DrawQuad(rocket.pos(), { rocketW, rocketH }, rocket.angle, rocket.col);
				Hazel::Renderer2D::EndScene();

				ImGui::DragFloat4("obstical", &obstacle.x, .05f, -rocket.aspect, rocket.aspect);
				ImGui::DragFloat2("rocketTarget", &Rocket::rocketTarget[0], .05f, -rocket.aspect, rocket.aspect);
			}
			else
			{
				if (testing > popsize - 1)
					testing = (int)popsize - 1;
				Rocket& rocket = pop.rocketPopulation[testing];
				ImGui::Text("stats of rocket %p:", &rocket);
				ImGui::SliderFloat2("pos: ", &rocket.Pos.x, -2, 2);
				ImGui::Text("vel: x~%f, y~%f", rocket.vel.x, rocket.vel.y);
				ImGui::Text("acc: x~%f, y~%f", rocket.acc.x, rocket.acc.y);
				ImGui::ColorEdit4("color:", rocket.col.GetRGBAPointer());
				ImGui::SliderFloat("mutationRate", &Rocket::mutationRate, 0, 1);
				ImGui::SliderFloat("mutationPow", &Rocket::mutationpow, -.001f, 1);
				ImGui::Text("angle: %f", rocket.angle);
				ImGui::Text("fitness: %f", rocket.fitness(&obstacle));

				ImGui::DragFloat4("obstical", &obstacle.x, .05f, -rocket.aspect, rocket.aspect);
				ImGui::DragFloat2("rocketTarget", &Rocket::rocketTarget[0], .05f, -rocket.aspect, rocket.aspect);
			}

			mouse = { Hazel::Input::GetMouseX() / window.GetWidth() * 2.f * window.GetAspectRatio() - window.GetAspectRatio(), -(Hazel::Input::GetMouseY() / window.GetHeight() * 2.f - 1) };
			if (Hazel::Input::IsMouseButtonPressed(Hazel::Mouse::ButtonLeft))
				obstacle = { mouse, obstacle.z, obstacle.w };


			ImGui::Separator();

			auto& stats = Hazel::Renderer2D::GetStats();

			ImGui::TextColored({ .8f, .2f, .2f, 1 }, "number of draw calls: %d", stats.drawCalls);
			ImGui::TextColored({ .8f, .2f, .2f, 1 }, "number of quads: %d", stats.quadCount);


			static float prevTs = timeStep;

			static bool vsync = true;
			if (ImGui::Checkbox("vsync?", &vsync))
			{
				Hazel::Application::Get().GetWindow().SetVSync(vsync);
				prevTs = 0.1f;
			}
			static short frames = 0;
			{
				float times = (1 / 6.0f) / (prevTs + 0.00001f);
				frames = (int)fmod(frames, times);
				if (++(frames) == 1)
					prevTs = timeStep;
			}
			ImGui::TextColored({ .8f, .2f, .8f, 1.f }, "frameRate: %f-fps\n\t%f-seconds/frame", 1 / prevTs, prevTs);

#else

			constexpr int range = 50;


			ImGui::SliderFloat("mean", &mean, 0, range);
			ImGui::SliderFloat("standard deviation", &sd, .2, range);
			if (dist->mean() != mean || dist->sigma() != sd)
			{
				free(dist);
				dist = new std::normal_distribution<double>(mean, 1 / sd);
			}


			std::stringstream normal;
			int list[range] = {};

			for (int i = 0; i < Rolls; i++)
			{
				double number = (*dist)(gen);
				if ((number >= 0) && (number < range))
					++list[int(number)];
			}
			normal << "normal dist (" << mean << ", " << sd << "):\n";

			for (int i = 0; i < range; i++)
			{
				normal << i << '-' << (i + 1) << ": ";
				normal << std::string(list[i] * stars / Rolls, '*') << "\n";
			}

			//auto zvalue = (mean - testValue) / sd;
			//std::string probability = std::to_string(std::erfc(-zvalue * M_SQRT1_2)/2.0);
			ImGui::SliderFloat("probability", &testValue, 0, range);
			//ImGui::TextColored(ImVec4(0, 1, 1, 1), probability.c_str());
			ImGui::TextColored(ImVec4(1, 0, 1, 1), normal.str().c_str());
#endif

			ImGui::End();
		}

	}

	void OnEvent(Hazel::Event& e)
	{
		m_Camera.OnEvent(e);
	}


private:
	Hazel::OrthographicCameraController m_Camera;

	population pop = population(rocketW, rocketH);

	float angle = 0;

	bool notPaused = false;

	int Rolls = 10000;
	int stars = 500;
	float mean = 5.0, sd = 2.0;
	float testValue = -.55f;
	float timeStep = 0.f;
	std::default_random_engine gen;
	std::normal_distribution<double>* dist = new std::normal_distribution<double>(mean, 1 / sd);

	Hazel::Ref<Hazel::Framebuffer> m_FrameBuffer;

};


class Sandbox : public Hazel::Application {
public:
	Sandbox()
		:Application({ "Sandbox" }) {//, 900, 500}) {
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {

	}
};

Hazel::Application* Hazel::CreateApplication(Hazel::ApplicationCommandLineArgs args) {
	return new Sandbox;
}