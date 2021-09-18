#include "Rocket.h"
#include <iostream>



Rocket::Rocket() :Pos(startingPos), vel(startingVel), acc(startingAcc)//, r(Rand(0, 1)), g(Rand(0, 1)), b(Rand(0, 1))
{
}

Rocket::Rocket(float width, float height)
	: Pos(startingPos), vel(startingVel), acc(startingAcc),
	width(width), height(height), timeTofinish(-1), col({Rand(0, 1), Rand(0, 1), Rand(0, 1), .3f})
{
}

Rocket::Rocket(DNA _dna) : Pos(startingPos), vel(startingVel), acc(startingAcc), dna(_dna), timeTofinish(-1)/*, r(Rand(0, 1)),
	g(Rand(0, 1)), b(Rand(0, 1))*/
{
}

Rocket::~Rocket()
{
	//remove(this->target);
}


glm::vec3 Rocket::rocketTarget = vec3(0, .6, 0);


auto Rocket::applyForce(vec2 force)
{
	acc += force;
}

bool Rocket::update(vec4* barrier)
{
	aspect = Hazel::Application::Get().GetWindow().GetAspectRatio() * .99f;

	//border
	if (Pos.x < -aspect || Pos.x > aspect || Pos.y < -1 || Pos.y > 1) {
		return false;
	}

	//barier collision
	if (barrier && Pos.x > barrier->x - barrier->z / 2.0 && Pos.x < barrier->x + barrier->z / 2.0 &&
		Pos.y > barrier->y - barrier->w / 2.0 && Pos.y < barrier->y + barrier->w / 2.0) {
		return false;
	}

	if (distance(Pos, vec2(rocketTarget.x, rocketTarget.y)) > targetSize) {}
		acc *= 0;
		try {
			setMag(dna.genes[count], .01);
			this->applyForce(dna.genes[count]);


			vel += acc;

			Pos += vel;
			angle = atan2(vel.y, vel.x);

			/*if (distance(Pos, vec2(target.x, target.y)) <= targetSize)
				Rocket::targetSize *= (Rocket::targetSize < .005) ? 1 : .9f;*/


			return true;
		}
		catch (...) {
			return true;
		}
	
	return false;
}

float Rocket::heading()
{
	return atan2(vel.x, vel.y);
}

int Rocket::count;
float Rocket::mutationRate = .001f;
float Rocket::mutationpow = .001f;
float Rocket::targetSize = .1f;

vec3 Rocket::pos() {
	return vec3(Pos.x, Pos.y, 0);
}

float Rocket::fitness(vec4* barrier)
{
	aspect = Hazel::Application::Get().GetWindow().GetAspectRatio() * .99f;

	float timing = 12000.f / (float)(DNA::lifespan * 3 - count * 2);
	//wandering
	if (Pos.x <= -aspect || Pos.x >= aspect || Pos.y <= -.99 || Pos.y >= .99)
	{
		Fitness = 25 / (distance(pos(), rocketTarget) + .25f) - timing;
	}
	//barrier
	else if (barrier && Pos.x > barrier->x - barrier->z / 2.0 && Pos.x < barrier->x + barrier->z / 2.0 &&
		Pos.y > barrier->y - barrier->w / 2.0 && Pos.y < barrier->y + barrier->w / 2.0)
	{
		Fitness = 20 / (distance(pos(), rocketTarget) + .07f) - timing;
	}
	//found target
	else
	{
		Fitness = 35 / (distance(pos(), rocketTarget) + .25f) - .2f * timing;
		timeTofinish = count;
	}
	return Fitness;
}

Rocket Rocket::crossover(const Rocket& b)
{
	auto child = this->dna;
	int middle = (int)Rand(DNA::lifespan, 0);
	for (int i = middle; i < DNA::lifespan; i++) {
		child.genes[i] = b.dna.genes[i];
		float mutationChance = Rand(1, -1);
		if (abs(mutationChance) < mutationRate)
			child.genes[i] += vec2((pow(1+mutationpow, mutationChance) - 1));
	}
	Rocket childRocket(child);
	childRocket.col.GetRGBAPointer()[0] = (Rand(0.f, 1.f) > .5) ? this->col.GetRGBAPointer()[0] : b.col.GetRGBA()[0];
	childRocket.col.GetRGBAPointer()[1] = (Rand(0, 1) > .5) ? this->col.GetRGBAPointer()[1] : b.col.GetRGBA()[1];
	childRocket.col.GetRGBAPointer()[2] = (Rand(0, 1) > .5) ? this->col.GetRGBAPointer()[2] : b.col.GetRGBA()[2];
	childRocket.col.a = (this->col.a + b.col.a) / 2.0f;

	/*float mutationChance = Rand(1, -1);
	if (abs(mutationChance) < mutationRate) {
		childRocket.col.GetRGBAPointer()[0] += (pow(mutationpow, mutationChance) - 1);
		childRocket.col.GetRGBAPointer()[0] = clamp(childRocket.col.GetRGBAPointer()[0], 0.f, 1.f);
	}

	mutationChance = Rand(1, -1);
	if (abs(mutationChance) < mutationRate) {
		childRocket.col.GetRGBAPointer()[1] += (pow(mutationpow, mutationChance) - 1);
		childRocket.col.GetRGBAPointer()[1] = clamp(childRocket.col.GetRGBAPointer()[1], 0.f, 1.f);
	}

	mutationChance = Rand(1, -1);
	if (abs(mutationChance) < mutationRate) {
		childRocket.col.GetRGBAPointer()[2] += (pow(mutationpow, mutationChance) - 1);
		childRocket.col.GetRGBAPointer()[2] = clamp(childRocket.col.GetRGBAPointer()[2], 0.f, 1.f);
	}*/

	float mutationChanceR = Rand(1, -1);
	float mutationChanceG = Rand(1, -1);
	float mutationChanceB = Rand(1, -1);
	float mutationChanceA = Rand(1, -1);
	{
		glm::vec4 mutantcolor = childRocket.col.GetVec4();
		if (abs(mutationChanceR) < mutationRate) {
			mutantcolor[0] += (pow(1+mutationpow, mutationChanceR) - 1);
			mutantcolor[0] = clamp(mutantcolor[0], 0.f, 1.f);
		}if (abs(mutationChanceG) < mutationRate) {
			mutantcolor[1] += (pow(1+mutationpow, mutationChanceR) - 1);
			mutantcolor[1] = clamp(mutantcolor[1], 0.f, 1.f);
		}if (abs(mutationChanceB) < mutationRate) {
			mutantcolor[2] += (pow(1+mutationpow, mutationChanceR) - 1);
			mutantcolor[2] = clamp(mutantcolor[2], 0.f, 1.f);
		}if (abs(mutationChanceA) < mutationRate) {
			mutantcolor[3] += (pow(1+mutationpow, mutationChanceR) - 1);
			mutantcolor[3] = clamp(mutantcolor[3], 0.f, 1.f);
		}
		if (mutantcolor != childRocket.col.GetVec4())
			childRocket.col = mutantcolor;
	}

	/*mutationChance = Rand(1, -1);
	if (abs(mutationChance) < mutationRate) {
		childRocket.mutationRate += (pow(mutationpow, mutationChance) - 1);
		childRocket.mutationRate = clamp(childRocket.mutationRate, 0.0001f, .9999f);
	}*/

	return childRocket;
}

Rocket& Rocket::operator=(const Rocket& R)
{
	if (&R == this) {
	}
	this->dna = R.dna;
	this->Pos = R.startingPos;
	this->vel = R.startingVel;
	this->acc = R.startingAcc;
	this->col = R.col;
	this->mutationRate = R.mutationRate;
	this->width = R.width;
	this->height = R.height;
	this->timeTofinish = -1;

	return *this;
}
