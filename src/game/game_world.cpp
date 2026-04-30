#include "game_world.h"

#include <wv/application.h>
#include <wv/serialize.h>
#include <wv/reflection/reflection.h>

#include "crate_controller.h"
#include <tuple>

void GameWorld::onSetupInput( wv::InputSystem* _inputSystem )
{ 
	wv::ActionGroup* playerActionGroup = _inputSystem->createActionGroup( "Player" );

	playerActionGroup->bindTriggerAction( "Shake", "Controller", wv::CONTROLLER_BUTTON_A );
	playerActionGroup->bindTriggerAction( "Shake", "Keyboard", wv::SCANCODE_SPACE );
	
	playerActionGroup->bindTriggerAction( "DebugMouseLock", "Keyboard", wv::SCANCODE_R );

	playerActionGroup->bindAxisAction( "Move", "Controller", wv::AXIS_DIRECTION_ALL, wv::CONTROLLER_JOYSTICK_LEFT );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_NORTH, wv::SCANCODE_W );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_SOUTH, wv::SCANCODE_S );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_EAST, wv::SCANCODE_D );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_WEST, wv::SCANCODE_A );

	playerActionGroup->bindAxisAction( "Look", "Controller", wv::AXIS_DIRECTION_ALL, wv::CONTROLLER_JOYSTICK_RIGHT );
	playerActionGroup->bindAxisAction( "Look", "Mouse", wv::AXIS_DIRECTION_ALL, wv::MOUSE_MOTION_AXIS );

	playerActionGroup->enable();
}

static void to_json( nlohmann::json& _json, const CrateComponent& _comp ) { 
	_json = nlohmann::json{
		// { "cameraSensitivity",  _comp.cameraSensitivity },
	};
}

static void from_json( const nlohmann::json& _json, CrateComponent& _comp ) {
	// _json.at( "cameraSensitivity" ).get_to( _comp.cameraSensitivity );
}

template <typename Ty>
constexpr inline const auto& reflection_of = Ty::reflection;

template <typename Ty, typename = int>
struct HasReflection : std::false_type { };

template <typename Ty>
struct HasReflection <Ty, decltype( (void)Ty::reflection, 0 )> : std::true_type { };

struct IField
{
	const char* name;

	constexpr IField( const char* _name ) : name{ _name } { }

	virtual nlohmann::json serialize( void* _ptr ) = 0;
};

template <typename Class, typename FieldTy>
struct Field : IField
{
	FieldTy Class::* ptr;

	constexpr Field( const char* _name, FieldTy Class::* _p ) noexcept;

	virtual nlohmann::json serialize( void* _ptr ) override {
		Class* c = (Class*)_ptr;

		if constexpr ( HasReflection<FieldTy>::value )
		{
			nlohmann::json j{};
			
			for ( const auto& r : reflection_of<FieldTy>.fields )
				j[ r->name ] = r->serialize( &( c->*ptr ) );

			return j;
		}
		else
		{
			// primitive or unreflected type
			return c->*ptr;
		}
	}
};

template<typename Class, typename FieldTy>
constexpr Field<Class, FieldTy>::Field( const char* _name, FieldTy Class::* _p ) noexcept
	: IField{ _name }, ptr{ _p }
{ }

template<typename Class, typename FieldTy>
constexpr IField* reflect( const char* _name, FieldTy Class::* _p )
{
	Field<Class, FieldTy>* f = new Field<Class, FieldTy>( _name, _p );
	return (IField*)f;
}

struct Reflection
{
	std::vector<IField*> fields;

	Reflection( std::initializer_list<IField*> _l ) {
		fields.insert( fields.end(), _l.begin(), _l.end() );
	}

	~Reflection() {
		for ( IField* f : fields )
			delete f;
	}

	nlohmann::json serialize( void* _ptr ) const {
		nlohmann::json j{};
		for ( auto& r : fields )
			j[ r->name ] = r->serialize( _ptr );
		return j;
	}
};

struct Test
{
	float asd = 983147.0f;
	static inline Reflection reflection{
		reflect( "asd", &Test::asd )
	};
};

struct Point
{
	float x, y;
	int z;
	Test t;

	static inline Reflection reflection{
		reflect( "x", &Point::x ),
		reflect( "y", &Point::y ),
		reflect( "z", &Point::z ),
		reflect( "test", &Point::t )
	};
};

void GameWorld::onSceneCreate()
{
	Point p{ 0.0f, 1.0f, 3, {} };
	nlohmann::json j = reflection_of<Point>.serialize( &p );
	
	printf( "%s\n", j.dump( 1 ).c_str() );
	
	// Components
	registerComponentType<CrateComponent>( "CrateComponent" );
	m_serializer->addComponentFunction<CrateComponent>();
	
	// Systems
	addSystem<CrateController>();
	
	// Load World
	load( "worlds/test_world.world" );
}

