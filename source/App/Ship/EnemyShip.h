#pragma once


#include "Ship.h"

class EnemyShip : public Ship
{
public:
	 EnemyShip( wv::Mesh* _mesh );
	~EnemyShip();

	void onCreate();
	void update( double _deltaTime ) override;

	void setTarget( wv::Vector3f _target ) { m_target = _target; }

private:
	wv::Vector3f m_target;

};