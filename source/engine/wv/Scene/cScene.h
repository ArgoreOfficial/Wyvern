#pragma once

struct sInputInfo; // change to event manager?

class cScene
{
public:
    cScene( void ) { }
    ~cScene( void ) { }

    void create ( void );                  
	void destroy( void );                 
    void update ( double _delta_time );
    void render ( void );                  
	void onRawInput( sInputInfo* _info );

private:

};

