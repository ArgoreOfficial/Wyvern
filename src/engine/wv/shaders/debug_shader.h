#pragma once

#include <wv/rendering/material.h>

namespace wv {

class DebugShader : public wv::IShader
{
public:
	struct DebugMaterialData
	{
		wv::Vector4f color;
	};

	virtual void initialize() override {
		setDebugName( "Debug" );
		setTopology( wv::TopologyClass::WV_LINE );
		setVertPath( "shaders/default_debug.vert.spv" );
		setFragPath( "shaders/default_debug.frag.spv" );
		create( sizeof( DebugMaterialData ) );
	}

	virtual void parseMaterialData( size_t _index, nlohmann::json _json ) override
	{
		DebugMaterialData data{};

		auto albedo = _json[ "color" ];
		if ( albedo.is_array() )
		{
			data.color = {
				albedo[ 0 ],
				albedo[ 1 ],
				albedo[ 2 ],
				albedo[ 3 ]
			};
		}

		setMaterialData( _index, &data );
	}
};
}