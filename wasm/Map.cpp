#include "Map.hpp"

bool Map::OnGround(const Rectangle& _pos) const {
	throw "new implimented yet";
	return true;
}

void Map::UpdatePosAndVel(Rectangle& _pos, glm::vec3& _vel, glm::vec3& _acc) const {
	return;
}

std::optional<Object*> Map::CanActivate(const glm::vec3& _pos, const Orientation _ori) const {
	throw "not implimented yet";
	return std::optional<Object*>();
}


