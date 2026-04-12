#pragma once

namespace wv {

class IUpdatable
{
	friend class World;

public:
	std::string getDebugName() const { return m_debugName; }

	virtual void onInitialize() { }
	virtual void onShutdown() { }

	virtual void onPreUpdate() { }
	virtual void onUpdate() { }
	virtual void onPostUpdate() { }
	
	virtual void onPhysicsUpdate() { }

	virtual void onDebugRender() { }
	virtual void onEditorRender() { }

	virtual void onRender() { }

	void setDebugRenderEnabled( bool _enabled ) { m_debugRenderEnabled = _enabled; }
	bool getDebugRenderEnabled()          const { return m_debugRenderEnabled; }

	void setEditorRenderEnabled( bool _enabled ) { m_editorRenderEnabled = _enabled; }
	bool getEditorRenderEnabled()          const { return m_editorRenderEnabled; }

protected:
	std::string m_debugName = "";

private:
	bool m_editorRenderEnabled = false;
	bool m_debugRenderEnabled = true;
	bool m_initalized = false;
};

}