#pragma once

#include "IComponent.h"

namespace wv
{

class ScriptComponent : public IComponent<ScriptComponent>
{
public:
	static const char* getName() { 
		return "ScriptComponent"; 
	}

	virtual FunctionFlags getFunctionFlags() override {
		return FunctionFlags::kOnEnter | FunctionFlags::kOnUpdate;
	}

	virtual void onEnter( void ) override;
	virtual void onUpdate( double _deltaTime ) override;
};

}