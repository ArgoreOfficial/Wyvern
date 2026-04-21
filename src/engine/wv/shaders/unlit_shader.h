#pragma once

#include <wv/rendering/material.h>

namespace wv {

class UnlitShader : public wv::IShader
{
public:
	struct UnlitMaterialData
	{
		Vector4f albedoColor;
		uint32_t albedoIndex;
		uint32_t pad0[ 3 ];
	};

	virtual void initialize() override {
		setDebugName( "Unlit" );
		setTopology( wv::TopologyClass::WV_TRIANGLE );
		setVertPath( "shaders/default_unlit.vert.spv" );
		setFragPath( "shaders/default_unlit.frag.spv" );
		create( sizeof( UnlitMaterialData ) );
	}

	virtual void parseMaterialData( size_t _index, nlohmann::json _json ) override
	{
		UnlitMaterialData data{};

		auto albedo = _json[ "albedo" ];
		if ( albedo.is_string() )
		{
			data.albedoIndex = 0;
			data.albedoColor = { 1.0f, 1.0f, 1.0f, 1.0f };

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