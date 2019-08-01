#include "Player.hpp"

void Player::Update(const Input& _input, const Map& _map, const unsigned long long _dT) {
	glm::vec3 acc; // acceleration
	if (_map.OnGround(m_pos)) {
		m_timeSinceLastGround = 0;
	} else {
		m_timeSinceLastGround += _dT;
	}

	std::optional<Object*>  optObj = _map.CanActivate(m_pos.Mid(), m_ori);	
	if (_input.action && optObj) {
		optObj.value()->Activate();
	}
	if (_input.left) {
		acc.x -= HorizontelSpeed;
	}
	if (_input.right) {
		acc.x += HorizontelSpeed;
	}
	if (_input.jump) {
		acc.y += VerticalSpeed(static_cast<float>(m_timeSinceLastGround) / 1000.f);  
	}
}
