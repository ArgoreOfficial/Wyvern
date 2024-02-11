#pragma once

struct sInputInfo;

class iScene
{
public:
    virtual ~iScene( void ) { }

    virtual void create ( void ) = 0;                  
	virtual void destroy( void ) = 0;                 
    virtual void update ( double _delta_time ) = 0;    
    virtual void render ( void ) = 0;                  
	virtual void onRawInput( sInputInfo* _info ) = 0;

protected:
    iScene( void ) { }

private:

};

