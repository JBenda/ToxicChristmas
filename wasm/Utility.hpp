#pragma once

#include <glm/vec3.hpp>
#include <cmath>

struct Input {
	Input(void** _link) {
		*_link = this;
	}
	Input() {}
	unsigned char jump,
	right,
	left,
	action,
	pause,
	cont, // contitue
	restart,
	level;
};

enum Orientation{Left, Right};

struct Rectangle {
	Rectangle(const glm::vec3& _tl, const glm::vec3& _bR) : topLeft{_tl}, bottomRight{_bR}{}
	glm::vec3 topLeft, bottomRight;

	bool Collide(const Rectangle& _rect) const {
		const float w = bottomRight.x - topLeft.x;
		const float h = topLeft.y - bottomRight.y;
		const glm::vec3 corners[] = {_rect.topLeft, _rect.topLeft + glm::vec3(w, 0, 0),
			_rect.topLeft + glm::vec3(-h,0,0), _rect.bottomRight};
		for(const glm::vec3* itr = corners; itr < corners + 4; ++itr) {
			if (Between(topLeft, bottomRight, *itr))
				return true;
		}
		return false;
	}
	glm::vec3 Mid() const {
		return  (topLeft + bottomRight) * 0.5f;
	}
private:
	static bool Between(const glm::vec3& _c1, const glm::vec3& _c2, const glm::vec3& _p) {
		const float w = abs(_c1.x - _c2.x);
		const float h = abs(_c1.y - _c2.y);
		return w >= abs(_c1.x - _p.x) && w >= abs(_c2.x - _p.x)
			&& h >= abs(_c1.y - _p.y) && h >= abs(_c2.y - _p.y);
	}
};
