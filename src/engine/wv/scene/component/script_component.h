#pragma once

#include "icomponent.h"

namespace wv
{

class ScriptComponent : public IComponent
{
public:
	ScriptComponent() : 
		IComponent( "ScriptComponent" )
	{ }

	virtual FunctionFlags getFunctionFlags() override {
		return FunctionFlags::kOnEnter | FunctionFlags::kOnUpdate;
	}

	virtual void onEnter( void ) override;
	virtual void onUpdate( double _deltaTime ) override;
};

}