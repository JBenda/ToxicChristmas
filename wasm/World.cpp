#include "World.hpp"
#include <thread>

void World::Update(const Input& _input, unsigned long long _dt) {
	if (m_modus != Modie::Load) {
		if (_input.level && (m_modus == Modie::Pause || m_modus == Modie::Unset)) {
			LoadLevel(_input.level);
		}
		if (_input.pause && m_modus == Modie::Run) {
			m_modus = Pause;
		}
		if (_input.cont && m_modus == Modie::Pause) {
			m_modus = Run;
		}
		if (_input.restart && m_modus != Modie::Unset) {
			LoadLevel(m_lvl);
		}
	}
	switch (m_modus) {
	case Modie::Run: 
		for(Object* obj : m_objs) {
			obj->Update(_dt);
		}
		m_toxic.Update(_dt);
		m_player.Update(_input, m_map, _dt);
		break;
	case Modie::Load: {
		// Loading Screen Stuff
		} break;
	case Modie::Pause: {
		// pause Srcen Stuff
		} break;
	case Modie::Unset: {
		// nothing??
		static int count = 0;
		if (_input.jump) {
			m_mView = glm::mat4(2.0f);
		} else {
			m_mView = glm::mat4(++count + 2);
		}
		} break;
	default: throw "Undefined state";
	}
	return m_modus;
}

void World::LoadLevel(unsigned int _num) {
	if (m_modus == Modie::Load) return;
	m_lvl = _num;
	m_modus = Modie::Load;
	std::thread(&World::LoadRotine, this, _num).detach();	
}
