#pragma once

#include "Utility.hpp"

class Object {
public:
	virtual void Activate() = 0;
	virtual bool IsTrigger() const = 0; 
	virtual const Rectangle& GetPos() const = 0; 
	virtual void Update(unsigned long long _dt) = 0;

};
