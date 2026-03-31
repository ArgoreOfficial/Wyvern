#pragma once

#include <wv/entity/ecs.h>

namespace wv {

class Entity;

struct OrbitControllerComponent;

class OrbitControllerSystem : public ISystem
{
public:
	virtual void configure( ArchetypeConfig& _config ) override;
	
	virtual void onUpdate() override;
private:

};

}