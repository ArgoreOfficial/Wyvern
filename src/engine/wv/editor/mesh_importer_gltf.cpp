#include "mesh_importer_gltf.h"

#include <wv/application.h>
#include <wv/rendering/mesh.h>
#include <wv/rendering/material.h>
#include <wv/rendering/texture.h>

#include <wv/filesystem/file_system.h>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

template <>
struct fastgltf::ElementTraits<wv::Vector3f> : fastgltf::ElementTraitsBase<wv::Vector3f, AccessorType::Vec3, float> { };

template <>
struct fastgltf::ElementTraits<wv::Vector2f> : fastgltf::ElementTraitsBase<wv::Vector2f, AccessorType::Vec2, float> { };

void wv::MeshImporterGLTF::load( const std::filesystem::path& _path, MeshManager* _meshManager, MaterialManager* _materialManager, TextureManager* _textureManager )
{
	// Load file
	IFileSystem* fs = Application::getSingleton()->getFileSystem();

	constexpr auto gltfOptions = 
		  fastgltf::Options::DontRequireValidAssetMember 
		| fastgltf::Options::AllowDouble 
		| fastgltf::Options::LoadGLBBuffers 
		| fastgltf::Options::LoadExternalBuffers;

	fastgltf::Parser parser;
	std::filesystem::path fullpath = fs->getFullPath( _path );

	fastgltf::Expected<fastgltf::GltfDataBuffer> data = fastgltf::GltfDataBuffer::FromPath( fullpath );
	if ( data.error() != fastgltf::Error::None )
	{
		WV_LOG_ERROR( "Failed to load model %s\n", _path.string().c_str() );
		return;
	}

	fastgltf::Expected<fastgltf::Asset> load = parser.loadGltf( data.get(), fullpath.parent_path(), gltfOptions );
	if ( load.error() != fastgltf::Error::None )
	{
		WV_LOG_ERROR( "Failed to parse model%s\n", _path.string().c_str() );
		return;
	}

	fastgltf::Asset& asset = load.get();

	if ( _meshManager->contains( _path ) )
	{
		m_meshAsset = _meshManager->get( _path );
	}
	else
	{
		parseMesh( asset );
	}

	std::vector<Ref<TextureAsset>> textures;

	for ( fastgltf::Image& image : asset.images )
	{

		std::visit( fastgltf::visitor {
            [](auto& arg) {},
            [&](fastgltf::sources::URI& _filePath) {
				if ( _filePath.fileByteOffset == 0 && _filePath.uri.isLocalPath() )
					textures.push_back( _textureManager->get( _filePath.uri.fspath() ) );
				else
					textures.push_back( {} );
            },
            [&](fastgltf::sources::Vector& _vector) {
				Ref<TextureAsset> texture = std::make_shared<TextureAsset>( (uint8_t*)_vector.bytes.data(), _vector.bytes.size() );
				textures.push_back( texture );
            },
            [&](fastgltf::sources::BufferView& view) {
				auto& bufferView = asset.bufferViews[ view.bufferViewIndex ];
				auto& buffer     = asset.buffers[ bufferView.bufferIndex ];
				
				std::visit( fastgltf::visitor{
					[]( auto& arg ) { },
					[ & ]( fastgltf::sources::Array& _array ) {
						Ref<TextureAsset> texture = std::make_shared<TextureAsset>( (uint8_t*)( _array.bytes.data() + bufferView.byteOffset ), bufferView.byteLength );
						textures.push_back( texture );
					} },
					buffer.data );
            },
        },
        image.data);
	}

	const char* shaderName = "Default Lit";

	for ( fastgltf::Material& mat : asset.materials )
	{
		Ref<MaterialAsset> newMat = _materialManager->get( shaderName );
		MaterialInstance instance{ newMat };

		/*
		file.materials[ mat.name.c_str() ] = newMat;

		GLTFMetallic_Roughness::MaterialConstants constants;
		constants.colorFactors.x = mat.pbrData.baseColorFactor[ 0 ];
		constants.colorFactors.y = mat.pbrData.baseColorFactor[ 1 ];
		constants.colorFactors.z = mat.pbrData.baseColorFactor[ 2 ];
		constants.colorFactors.w = mat.pbrData.baseColorFactor[ 3 ];

		constants.metal_rough_factors.x = mat.pbrData.metallicFactor;
		constants.metal_rough_factors.y = mat.pbrData.roughnessFactor;
		// write material parameters to buffer
		sceneMaterialConstants[ data_index ] = constants;

		MaterialPass passType = MaterialPass::MainColor;
		if ( mat.alphaMode == fastgltf::AlphaMode::Blend )
		{
			passType = MaterialPass::Transparent;
		}

		GLTFMetallic_Roughness::MaterialResources materialResources;
		// default the material textures
		materialResources.colorImage = engine->_whiteImage;
		materialResources.colorSampler = engine->_defaultSamplerLinear;
		materialResources.metalRoughImage = engine->_whiteImage;
		materialResources.metalRoughSampler = engine->_defaultSamplerLinear;

		// set the uniform buffer for the material data
		materialResources.dataBuffer = file.materialDataBuffer.buffer;
		materialResources.dataBufferOffset = data_index * sizeof( GLTFMetallic_Roughness::MaterialConstants );
		// grab textures from gltf file
		*/
		if ( mat.pbrData.baseColorTexture.has_value() )
		{
			size_t img = asset.textures[ mat.pbrData.baseColorTexture.value().textureIndex ].imageIndex.value();
			// size_t sampler = asset.textures[ mat.pbrData.baseColorTexture.value().textureIndex ].samplerIndex.value();

			instance.textures.push_back( textures[ img ] );
			instance.setValue( "albedoIndex", textures[ img ]->getImageSlot() );

		}
		
		m_materials.push_back( instance );
	}
	
	// there must always be at least one material
	if ( m_materials.size() == 0 )
		m_materials.push_back( _materialManager->get( shaderName ) );

}

void wv::MeshImporterGLTF::parseMesh( fastgltf::Asset& _asset )
{
	// Parse gltf

	GeometrySurface surface{};

	for ( fastgltf::Mesh& mesh : _asset.meshes )
	{
		for ( auto& primitive : mesh.primitives )
		{
			uint16_t indexOffset = surface.vertexCount();

			std::vector<uint16_t> indices;
			std::vector<Vector3f> positions;
			std::vector<Vector3f> normals;
			std::vector<Vector2f> uv0s;
			std::vector<Vector3f> colours;

			fastgltf::Attribute* posAttrib = primitive.findAttribute( "POSITION" );
			fastgltf::Attribute* norAttrib = primitive.findAttribute( "NORMAL" );
			fastgltf::Attribute* uv0Attrib = primitive.findAttribute( "TEXCOORD_0" );
			fastgltf::Attribute* colAttrib = primitive.findAttribute( "COLOR_0" );

			// load indices
			{
				fastgltf::Accessor& indexAccessor = _asset.accessors[ primitive.indicesAccessor.value() ];
				indices.resize( indexAccessor.count );
				fastgltf::copyFromAccessor<uint16_t>( _asset, indexAccessor, indices.data() );
			}

			// load vertex data
			{
				// vertex position
				if ( posAttrib != primitive.attributes.end() )
				{
					fastgltf::Accessor& positionAccessor = _asset.accessors[ posAttrib->accessorIndex ];
					positions.resize( positionAccessor.count );
					fastgltf::copyFromAccessor<Vector3f>( _asset, positionAccessor, positions.data() );
				}

				// vertex normal
				if ( norAttrib != primitive.attributes.end() )
				{
					fastgltf::Accessor& normalAccessor = _asset.accessors[ norAttrib->accessorIndex ];
					normals.resize( normalAccessor.count );
					fastgltf::copyFromAccessor<Vector3f>( _asset, normalAccessor, normals.data() );
				}

				// texture coordinate 0
				if ( uv0Attrib != primitive.attributes.end() )
				{
					fastgltf::Accessor& texCoord0Accessor = _asset.accessors[ uv0Attrib->accessorIndex ];
					uv0s.resize( texCoord0Accessor.count );
					fastgltf::copyFromAccessor<Vector2f>( _asset, texCoord0Accessor, uv0s.data() );
				}

				// vertex colour 0
				if ( colAttrib != primitive.attributes.end() )
				{
					fastgltf::Accessor& colourAccessor = _asset.accessors[ colAttrib->accessorIndex ];
					colours.resize( colourAccessor.count );
					fastgltf::copyFromAccessor<Vector3f>( _asset, colourAccessor, colours.data() );
				}
			}

			GeometrySurface::Primitive prim{};
			prim.firstIndex   = surface.indices.size();
			prim.vertexOffset = surface.vertexCount();
			prim.vertexCount  = positions.size();
			prim.indexCount   = indices.size();

			if ( primitive.materialIndex.has_value() )
				prim.material = *primitive.materialIndex;
			else
			{
				WV_LOG_WARNING( "%s does not have a material. Using default\n", mesh.name.c_str() );
				prim.material = 0;
			}

			surface.primitives.push_back( prim );

			surface.vertexPositions.insert( surface.vertexPositions.end(), positions.begin(), positions.end() );
			surface.vertexNormals.insert( surface.vertexNormals.end(), normals.begin(), normals.end() );
			surface.vertexUVs.insert( surface.vertexUVs.end(), uv0s.begin(), uv0s.end() );
			surface.vertexColours.insert( surface.vertexColours.end(), colours.begin(), colours.end() );

			for ( uint16_t index : indices )
				surface.indices.push_back( index + indexOffset );
		}
	}

	fastgltf::iterateSceneNodes( _asset, 0, fastgltf::math::fmat4x4(),
		[ & ]( fastgltf::Node& _node, fastgltf::math::fmat4x4 _matrix ) {
		if ( _node.meshIndex.has_value() )
		{
			wv::Matrix4x4f matrix( _matrix.data() );
			wv::Matrix3x3f normalMatrix = {};// transpose(inverse(mat3(modelMatrix)))
			normalMatrix.setRow( 0, { matrix.get( 0, 0 ), matrix.get( 0, 1 ), matrix.get( 0, 2 ) } );
			normalMatrix.setRow( 1, { matrix.get( 1, 0 ), matrix.get( 1, 1 ), matrix.get( 1, 2 ) } );
			normalMatrix.setRow( 2, { matrix.get( 2, 0 ), matrix.get( 2, 1 ), matrix.get( 2, 2 ) } );

			normalMatrix = wv::Math::transpose( wv::Math::inverse( normalMatrix ) );

			auto prim = surface.primitives[ *_node.meshIndex ];

			for ( size_t i = 0; i < prim.vertexCount; i++ )
			{
				auto& vec = surface.vertexPositions[ i + prim.vertexOffset ];
				vec = vec * matrix;

				auto& norm = surface.vertexNormals[ i + prim.vertexOffset ];
				norm = norm * normalMatrix;
				norm.normalize();
			}
		}
	} );

	m_meshAsset = std::make_shared<MeshAsset>();
	m_meshAsset->initialize( surface );
}
