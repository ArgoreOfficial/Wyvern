#pragma once

struct sInputInfo;

class iScene
{
public:
    virtual ~iScene( void ) { }

    virtual void create( void ) { }
	virtual void destroy( void ) { }
    virtual void update( double _delta_time ) { }
    virtual void draw  ( void ) { }
	virtual void onRawInput( sInputInfo* _info ) { };
protected:
    iScene( void ) { }

private:

};

