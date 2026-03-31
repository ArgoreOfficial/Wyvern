#pragma once

namespace wv {

class IUpdatable
{
	friend class World;

public:
	virtual void onInitialize() { }
	virtual void onShutdown() { }

	virtual void onPreUpdate() { }
	virtual void onUpdate() { }
	virtual void onPostUpdate() { }

private:
	bool m_initalized = false;
};

}