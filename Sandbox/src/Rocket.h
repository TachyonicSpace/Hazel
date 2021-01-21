#pragma once
#include "DNA.h"
using namespace glm;

#define print(vec) (vec).x << "\t" << (vec).y

#define setMag(x, mag) x*=(mag/length(x))

class Rocket
{

public:
	vec3 target = vec3(0, .6, 0);
	const vec2 startingPos = vec2(0, .10 - 1);
	const vec2 startingVel = vec2(0, 0);
	const vec2 startingAcc = vec2(0, 0);

	vec2 Pos;
	vec2 vel;
	vec2 acc;
	DNA dna;

	Hazel::Color col;
	float width = 25;
	float height = 5;
	static float targetSize;
	float aspect = 0;

	float angle = 0;
	float Fitness = 0;
	static int count;
	static float mutationRate;
	static float mutationpow;
	static float* rect(float width, float height);
	int timeTofinish = -1;

	Rocket();
	Rocket(float width, float height);
	Rocket(DNA _dna);
	~Rocket();

	auto applyForce(vec2 force);

	bool update(vec4* barrier = nullptr);

	float heading();

	vec3 pos();

	float fitness(vec4* barrier);

	Rocket crossover(const Rocket& b);

	Rocket& operator=(const Rocket& R);

	float* rect();
};