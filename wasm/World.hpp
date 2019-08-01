#pragma once

#include <vector>

#include "glmIncludes.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "GasSystem.hpp"

class World {
public:
	World(void** _viewMat, Player& _player) : m_player{_player}, m_mView(1.0f),
		m_modus{Modie::Unset} {
		*_viewMat = glm::value_ptr(m_mView);
	}
	void LoadLevel(unsigned int _num);
	void Update(const Input& _input, unsigned long long _dt);
private:
	glm::mat4 m_mView;
	Player& m_player;
	Map m_map;
	std::vector<Object*> m_objs;
	GasSystem m_toxic;
	enum Modie {Unset, Run, Pause, Load} m_modus;
	unsigned int m_lvl;

	void LoadRotine(unsigned int _num) {
		m_modus = Modie::Run;
	};
};
