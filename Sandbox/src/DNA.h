#pragma once

#include <Hazel.h>

#include "glm/glm.hpp"

#define PI 3.14159265358979f 
#define random() (2* PI *rand() / ((float)RAND_MAX))
#define Rand(max, min) (((float)rand()*((float)(((float)max)-((float)min))/((float)RAND_MAX)))+(float)(min))
#define mutate(x, amount) x += Rand(amount, -amount)



class DNA
{
public:
	static const int lifespan = 100;
	glm::vec2 genes[lifespan];
	DNA();
};