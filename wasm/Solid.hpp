#pragma once

#include "Object.hpp"
#include "glmIncludes.hpp"

class Solid : Object {
public:
	Object(Rectangle& pos) m_pos{pos} {};
	override void Update(unsigned long long _dt) {}
	override const Rectangle& GetPos() const {return m_pos;}
	override bool IsTrigger() const { return true;}
	override void Activate() { throw "Can't be Activatied.";}
private:
	const Rectangle& m_pos;
};
