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

	virtual void parseMaterialData( MaterialAsset& _material, nlohmann::json _json ) override
	{
		UnlitMaterialData data{};

		auto albedo = _json[ "albedo" ];
		if ( albedo.is_string() )
		{
			data.albedoColor = { 1.0f, 1.0f, 1.0f, 1.0f };

			Ref<TextureAsset> texture = TextureAsset::get( albedo.get<std::string>() );
			if ( texture )
			{
				data.albedoIndex = texture->getImageSlot();
				_material.textureAssets.push_back( texture );
			}
			else
			{
				data.albedoIndex = 0;
			}
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

		setMaterialData( _material, &data );
	}
};
}