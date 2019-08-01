#pragma once

#include <optional>

#include "glmIncludes.hpp"
#include "Utility.hpp"
#include "Object.hpp"

// World Decokrater
class Map {
public:
	Map() {}
	bool OnGround(const Rectangle& _pos)const;
	void UpdatePosAndVel(Rectangle& _pos, glm::vec3& _vel, glm::vec3& _acc ) const;
	std::optional<Object*> CanActivate(const glm::vec3& _pos, const Orientation _ori) const;
private:
};
