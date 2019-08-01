#pragma once

#include "glmIncludes.hpp"
#include "Utility.hpp"
#include "Map.hpp"

namespace glm {
	typedef glm::mat4x4 mat4;
};

class Player {
	static constexpr float HorizontelSpeed = 3.0f;
	static float VerticalSpeed(float t) { return 20.0f * pow(2.7f,-t); }
public:
	Player(void ** _viewMat) : m_mView(1.0f), m_live{1.f}, m_pos(glm::vec3(0), glm::vec3(1,1,1)) {
		*_viewMat = glm::value_ptr(m_mView);	
	}
	void Update(const Input& _input, const Map& _map, const unsigned long long _dT);
	const Rectangle& GetPos() const {return m_pos;}
	void AddToxic(float _val) { m_live -= _val;}
	void ResetState() {
		m_live = 1.f;
	}

	void SetPosition(glm::vec3 _pos) {
		m_pos.topLeft -= m_pos.bottomRight;
		m_pos.bottomRight = _pos;
		m_pos.topLeft += m_pos.bottomRight;
	}
private:
	float m_live;
	Rectangle m_pos;
	glm::vec3 m_vel;
	glm::mat4 m_mView;
	Orientation m_ori;
	unsigned int m_timeSinceLastGround;
};
