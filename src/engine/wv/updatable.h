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

	virtual void onDebugRender() { }
	virtual void onRender() { }

private:
	bool m_initalized = false;
};

}