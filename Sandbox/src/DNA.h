#pragma once

#include <Hazel.h>

#include "glm/glm.hpp"

#define PI 3.14159265358979f 
#define random() (2* PI *rand() / ((float)(RAND_MAX)))
#define Rand(max, min) (rand()*(((max)-(min))/((float)RAND_MAX)))+(min)
#define mutate(x, amount) x += Rand(amount, -amount)
//#define remove(x) delete(&x);\
					free(&x)


class DNA
{
public:
	static const int lifespan = 400;
	glm::vec2 genes[lifespan];
	DNA();
};