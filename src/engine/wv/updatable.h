#pragma once

namespace wv {

enum UpdateMode
{
	UpdateMode_Runtime = 1 << 0,
	UpdateMode_Editor  = 1 << 1,
	UpdateMode_Always  = UpdateMode_Runtime | UpdateMode_Editor,
};

class IUpdatable
{
	friend class World;

public:
	virtual void onInitialize() { }
	virtual void onShutdown() { }

	virtual void onPreUpdate() { }
	virtual void onUpdate() { }
	virtual void onPostUpdate() { }
	
	virtual void onPhysicsUpdate() { }

	virtual void onDebugRender() { }
	virtual void onEditorRender() { }

	virtual void onRender() { }
	
	std::string getDebugName()                     const { return m_debugName; }
	void        setDebugName( const std::string& _name ) { m_debugName = _name; }

	bool getDebugRenderEnabled()          const { return m_debugRenderEnabled; }
	void setDebugRenderEnabled( bool _enabled ) { m_debugRenderEnabled = _enabled; }

	bool getEditorRenderEnabled()          const { return m_editorRenderEnabled; }
	void setEditorRenderEnabled( bool _enabled ) { m_editorRenderEnabled = _enabled; }

	UpdateMode getUpdateMode()             const { return m_updateMode; }
	void       setUpdateMode( UpdateMode _mode ) { m_updateMode = _mode; }

	World* getWorld() const { return m_world; }

protected:
	std::string m_debugName = "";

private:
	UpdateMode m_updateMode = UpdateMode_Runtime;
	World* m_world = nullptr;

	bool m_editorRenderEnabled = false;
	bool m_debugRenderEnabled = true;
	bool m_initialized = false;
};

}