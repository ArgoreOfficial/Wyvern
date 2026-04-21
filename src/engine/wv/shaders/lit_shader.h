#pragma once

#include <wv/rendering/material.h>

namespace wv {

class LitShader : public wv::IShader
{
public:
	struct LitMaterialData
	{
		Vector4f albedoColor;
		uint32_t albedoIndex;
		uint32_t pad0[ 3 ];
	};

	virtual void initialize() override {
		setDebugName( "Lit" );
		setTopology( wv::TopologyClass::WV_TRIANGLE );
		setVertPath( "shaders/default_lit.vert.spv" );
		setFragPath( "shaders/default_lit.frag.spv" );
		create( sizeof( LitMaterialData ) );
	}

	virtual void parseMaterialData( size_t _index, nlohmann::json _json ) override
	{
		LitMaterialData data{};

		auto albedo = _json[ "albedo" ];
		if ( albedo.is_string() )
		{
			data.albedoColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			data.albedoIndex = 0;

			// get texture from albedo path
		}
		else if ( albedo.is_array() )
		{
			data.albedoIndex = 2;
			data.albedoColor = {
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