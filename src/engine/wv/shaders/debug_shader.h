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
		setTopology( wv::TopologyClass::TopologyClass_Line );
		setVertPath( "shaders/default_debug.vert.spv" );
		setFragPath( "shaders/default_debug.frag.spv" );
		create( sizeof( DebugMaterialData ) );
	}

	virtual void parseMaterialData( MaterialAsset& _material, nlohmann::json _json ) override
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

		setMaterialData( _material, &data );
	}
};
}