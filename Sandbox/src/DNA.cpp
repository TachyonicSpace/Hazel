#include "DNA.h"

DNA::DNA()
{
	for (int i = 0; i < lifespan; i++) {
		auto angle = random();
		genes[i] = glm::vec2(.1 * cos(angle), .1 * sin(angle));
	}
}